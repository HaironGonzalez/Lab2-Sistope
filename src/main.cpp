#include "bibliotecas.h"


int main (int argc, char **argv)
{
  int NumDiscos = 0;
  char * NomFichero_g = NULL;
  char * NomFichero_e = NULL;
  char * NomFichero_r = NULL;
  char * NomFichero_R = NULL;
  bool Cflag = false;
  bool Eflag = false;
  bool gflag = false;
  bool eflag = false;
  bool rflag = false;
  bool Rflag = false;
  bool mflag = false;
  char *aux = NULL;
  int c;
  int cont =0;
  opterr = 0;

  while ((c = getopt (argc, argv, "C:g:e:r:R:mE")) != -1)
  {
    switch (c)
    {
      case 'C':
        Cflag = true;
        aux = optarg;
        cont++;
        NumDiscos = atoi(aux);

        break; 
      case 'E':
        Eflag = true;
        cont++;
        break;
      case 'g':
        gflag = true;
        cont++;
        NomFichero_g = optarg;
        break;
      case 'e':
        eflag = true;
        cont++;
        NomFichero_e = optarg;
        break;
      case 'r':
        rflag = true;
        cont++;
        NomFichero_r = optarg;
        break;
      case 'R':
        Rflag = true;
        cont++;
        NomFichero_R = optarg;
        break;
      case 'm':
        mflag = true;
        cont++;
        break;
      case '?':
        if (optopt == 'c') cout << "Error de parámetros\n";
        else if (isprint (optopt)) cout << "Error banderas\n";
        else cout << "Error banderas\n";
        return 1;
      default:
        abort ();
    }
  }

  FILE *f;
  f = fopen("Config","r");
  if (cont != 1)
  {
    cout << "Error Banderas"<<endl;
    exit(0);
  }
  if (f != NULL && Cflag)
  {              // hay fichero y bandera
    cout << "Error: Ya hay un sistema de almacenamiento creado"<<endl;
    exit (0);
  }
  else if (Cflag)
  {                      // solo hay bandera 
    int pid;
    int NumPipes = NumDiscos * 2;
    int fd [NumPipes][2];
    int NumProceso;
    double largo;
    int NumPartes;
    vector <int> Leer;
    vector <int> Escribir;
    int EscribirH;
    int LeerH;
    ofstream config;
    config.open("Config");

    for(int i=0;i<NumPipes;i++)
    {
      if (pipe(fd[i]) == -1)
      {
        cout << "Error al crear los pipes \n";
        exit(1);
      }
    }

    for (int i=0;i<NumDiscos;i++)
    {     // Se crean los hijos  
      pid = fork();
      if(pid == 0)
      {
        close(fd[2*i][0]);    // 0 entrada 1 salida
        close(fd[2*i+1][1]);
        LeerH = fd[2*i+1][0];
        EscribirH = fd[2*i][1];
        NumProceso = i;
        //cout << "soy hijo "<<i<<endl;
        break;
      }
      else 
      {
        close(fd[2*i][1]);
        close(fd[2*i+1][0]);
        Leer.push_back(fd[2*i][0]);
        Escribir.push_back(fd[2*i+1][1]);
        pid =1;
      }
    } 

    char papiahijo[100];
    char hijoapapi[100];

    if(pid==0){ // hijos crean su disco 

      stringstream comando;
      stringstream msg;
      comando << "mkdir ./Discos\\ RAID/Disco\\ " << NumProceso;
      system (comando.str().c_str());

      msg << "Proceso "<< NumProceso << " Listo";
      write(EscribirH,msg.str().c_str(), strlen(msg.str().c_str())+1);
    }
    else if(pid == 1){

      config <<NumDiscos<<"\n";

      for (int i=0;i<NumDiscos;i++){   // espera a que todos los procesos creen su Disco 
        read(Leer[i],hijoapapi, sizeof(hijoapapi));
        //cout << hijoapapi <<endl;
      }
      cout << "Se crearon "<<NumDiscos<<" Discos RAID"<<endl;
    }   
  }
  else if(f != NULL){                   // solo hay fichero
    
    
    ifstream iconfig;
    string palabra;
    int pid;
    int NumProceso;
    int NumPartes;
    double largo;
    ofstream oconfig;


    iconfig.open("Config");       // rescata NumDiscos de Config
    iconfig >> palabra;
    NumDiscos = atoi(palabra.c_str());
    iconfig.close();


    vector <int> Leer;
    vector <int> Escribir;
    int LeerH;
    int EscribirH;
    int NumPipes = NumDiscos * 2;
    int fd [NumPipes][2];

    for(int i=0;i<NumPipes;i++)
    {
      if (pipe(fd[i]) == -1)
      {
        perror("No se pudo crear el pipe\n");
        exit(1);
      }
    }

    for (int i=0;i<NumDiscos;i++)
    {     // Se crean los hijos
      
      pid = fork();
      if(pid == 0)
      {
        close(fd[2*i][0]);    // 0 entrada 1 salida
        close(fd[2*i+1][1]);
        LeerH = fd[2*i+1][0];
        EscribirH = fd[2*i][1];
        NumProceso = i;
        //cout << "soy hijo "<<i<<endl;
        break;
      }
      else
      {

        close(fd[2*i][1]);
        close(fd[2*i+1][0]);
        Leer.push_back(fd[2*i][0]);
        Escribir.push_back(fd[2*i+1][1]);      
        pid = 1;
      }
    } 
    
    if (gflag)
    {
  
      if(pid==0)
      {

        //cout << "Proceso "<<NumProceso<<" Activo"<<endl;
        while(true){

          char mensaje[1024];
          char NomNuevo[100];
          char NomParticion[100];
          read(LeerH,NomNuevo, sizeof(NomNuevo));
          //cout << "Nombre particion "<<NomNuevo<< "En Disco "<< NumProceso<<endl;
          
          if (strcmp (NomNuevo,"Salir")==0){
            //cout << "soy proceso "<< NumProceso << "y sali\n";
            break;
          }
          sprintf(NomParticion, "./Discos RAID/Disco %d/%s",NumProceso,NomNuevo);
          //cout << NomParticion<<endl;    
          write(EscribirH,"recibi",7); 

          read(LeerH,mensaje, sizeof(mensaje));

          FILE *in;                                  // crea el particion indicada
          in = fopen (NomParticion,"wb");
          if(in==NULL)
          {
            perror("No se pudo crear una partición\n");
            exit(1);
          }
          fwrite (mensaje, 1,1024, in);
          fclose (in);
        }
      }
      else if (pid == 1){
        
        char nombre[200];
        
        sprintf(nombre, "./Archivos Originales/%s",NomFichero_g);
        //cout << "mandosd"<<NomFichero_g<<endl;
        FILE *f;
        f = fopen (nombre,"rb");
        if(f==NULL){
          cout << "Error: No se pudo abrir el fichero, verifique carpeta Archivos Originales y nombre de fichero"<<endl;
          exit(0);
        }
        fseek(f, 0L, SEEK_END);
        largo = ftell(f);
        //cout << "el largo es "<<largo<<endl;
        fclose(f);
        int i=0;
        
        f = fopen (nombre,"rb");
        int cont = 0;
        char hijoapapi[100];
        iconfig.open("Config");

        while (!iconfig.eof()){
          string palabra;
          iconfig >> palabra;
          iconfig >> palabra;

          if (strcmp(palabra.c_str(),NomFichero_g)==0){
            cout << "Error: Este fichero ya existe en el sistema"<<endl;
            exit(0);
          }
        }
        while(i<largo){

          for(int j =0;j<NumDiscos/2;j++){
            
            if (i<largo){
              
              //--------------------------- Agregar 0s al numero de fichero
              stringstream ceros;
              agregarceros(cont,largo,ceros,true);
              //--------------------------------
              char contenido[1024];
              fread (contenido, 1, 1024, f);
              stringstream NomParticion;
              
              NomParticion << NomFichero_g <<"-"<<ceros.str().c_str()<<cont;
              
              write(Escribir[j],NomParticion.str().c_str(),strlen(NomParticion.str().c_str())+1);
              write(Escribir[j+NumDiscos/2],NomParticion.str().c_str(),strlen(NomParticion.str().c_str())+1);
              //cout << contenido;
              //cout << "mando "<<NomParticion.str().c_str()<<endl;
              read (Leer[j],hijoapapi, sizeof(hijoapapi));            //espera la respuesta de recibi de los procesos para volver a escribir
              read (Leer[j+NumDiscos/2],hijoapapi, sizeof(hijoapapi));
              //cout <<hijoapapi;

              write(Escribir[j],contenido,1024);
              write(Escribir[j+NumDiscos/2],contenido,1024); 
              i = i + 1024;
              cont++;
            }
            else break;
          }
        }
        for(int i = 0;i<NumDiscos;i++){
          write(Escribir[i],"Salir",6);
        }
        cout<< "Fichero guardado exitosamente"<<endl;
        stringstream comando;
        comando << "rm ./Archivos\\ Originales/"<<NomFichero_g;
        system(comando.str().c_str());
        NumPartes = cont;
        oconfig.open("Config",ios::app);
        oconfig << NomFichero_g << " "<< NumPartes<< "\n";
        oconfig.close();
      } // guardar fichero en discos, se registra en config 
    }

    if(Eflag)
    {  // papi le dice eliminar a hijos y ellos borran carpetas, papi borra archivo config

      if (pid == 0) { 
        char mensaje[9];
        read(LeerH,mensaje,9);
        if  (strcmp(mensaje,"Eliminen")==0)
        {
          stringstream comando;
          comando << "rm -R ./Discos\\ RAID/Disco\\ "<< NumProceso;
          system (comando.str().c_str());
        }
      }
      else {

        for(int i = 0;i<NumDiscos;i++){
          write(Escribir[i],"Eliminen",9);
        }
        system ("rm ./Config");
      }
    }

    if (eflag){       // eliminar fichero al azar, no se registra en config


      if(pid == 0)
      {
        char NomNuevo[100];
        char NomaEliminar[100];
        read(LeerH,NomNuevo, sizeof(NomNuevo));
        //cout << "Mi papi dijo "<< NomNuevo<<endl;
        //cout << "Nombre particion "<<NomNuevo<< "En Disco "<< NumProceso<<endl;
        sprintf(NomaEliminar, "./Discos RAID/Disco %d/%s",NumProceso,NomNuevo);

        ifstream f;
        f.open(NomaEliminar);
        if(strcmp(NomNuevo,"")==0){
          //cout << "no lei nada "<<NumProceso<<endl;
          exit(0);
        }
        else if(!f.good()){
          //perror("NOO tengo ese archivo");
          write(EscribirH,"NO",3);
           
        }
        else{
          f.close();
          stringstream comando;
          comando << "rm ./Discos\\ RAID/Disco\\ "<<NumProceso <<"/"<<NomNuevo;
          system(comando.str().c_str());
          cout << "Se elimino "<<NomNuevo<<" de Disco "<< NumProceso<<" exitosamente"<<endl;
          write(EscribirH,"SI",3);
        }

        //cout << "mori "<<NumProceso<<endl;
        return 0;
      }
      else if(pid == 1)
      {
        iconfig.open("Config");
        bool check = false;
        while(!iconfig.eof()){
          string palabra;
          iconfig >> palabra;
          //cout << "Palabra es: "<<palabra<<endl;
          if (palabra.compare(NomFichero_e)==0){
            iconfig >> palabra;
            NumPartes = atoi(palabra.c_str());
            check = true;
            //cout << "el fichero "<< NomFichero_e << " tiene "<< NumPartes << "partes \n";
            break;

            //cout << "El archivo SSSSSSI esta en el sistema"<<endl;
          }
        }
        if (!check){
          cout << "Error: El archivo no está en el sistema"<<endl;
          exit(0);
        }

        srand(time(0));
        int aleatorio = rand() % NumPartes;
        srand(time(0));
        int OriginaloRespaldo = rand() % 2 + 1;
        stringstream ParEliminar;
        //--------------------------- Agregar 0s al numero de fichero
        stringstream ceros;
        agregarceros(aleatorio,NumPartes,ceros,false);
        //--------------------------------

        int i;
        ParEliminar << NomFichero_e <<"-"<<ceros.str().c_str()<<aleatorio;
        //cout << "hay que eliminar "<< ParEliminar.str().c_str()<<endl;
        char respuesta[3];
        if (OriginaloRespaldo == 1){

          for (i=0;i<NumDiscos/2;i++){
            write(Escribir[i],ParEliminar.str().c_str(),strlen(ParEliminar.str().c_str())+1);
          }
          for (i=0;i<NumDiscos/2;i++){
            read(Leer[i],respuesta, sizeof(respuesta));
            if(strcmp("SI",respuesta)==0)exit(0);
          }
          cout << "Archivo aleatorio a eliminar: "<< ParEliminar.str()<<", No exite en Discos Principales"<<endl;
        }
        else
        {
          for (i=0;i<NumDiscos/2;i++)
          {
            write(Escribir[i+NumDiscos/2],ParEliminar.str().c_str(),strlen(ParEliminar.str().c_str())+1);
          }
          for (i=0;i<NumDiscos/2;i++)
          {
            read(Leer[i+NumDiscos/2],respuesta, sizeof(respuesta));
            if(strcmp("SI",respuesta)==0)exit(0);
          }
          cout << "Archivo aleatorio a eliminar: "<< ParEliminar.str()<<", No exite en Discos de Respaldo"<<endl;
        }

        return 0;
      }   
    }

    if(rflag){
      int i = 0;
      if (pid == 1){


        iconfig.open("Config");
        bool check = false;
        while(!iconfig.eof()){
          string palabra;
          iconfig >> palabra;
          if (palabra.compare(NomFichero_r)==0){
            iconfig >> palabra;
            NumPartes = atoi(palabra.c_str());
            check = true;
            //cout << "el fichero "<< NomFichero_r << " tiene "<< NumPartes << "partes \n";
            //cout << "El archivo SSSSSSI esta en el sistema"<<endl;
            break;
          }
        }

        if (!check){
          cout << "Error: El archivo no esta en el sistema"<<endl;
          exit(0);
        }
        
        i=0;
        while(i<NumPartes){

          for(int j =0;j<NumDiscos/2;j++){
            char contenido [1024];
            if (i<NumPartes){
              
              //--------------------------- Agregar 0s al numero de fichero
              stringstream ceros;
              agregarceros(i,NumPartes,ceros,false);
              //--------------------------------
              stringstream NomParticion;
              char res1[3];
              char res2[3];
              char msj[20];
              NomParticion << NomFichero_r <<"-"<<ceros.str().c_str()<<i; 

              write(Escribir[j],NomParticion.str().c_str(),strlen(NomParticion.str().c_str())+1);
              write(Escribir[j+NumDiscos/2],NomParticion.str().c_str(),strlen(NomParticion.str().c_str())+1);
              //cout << "soy papi  y envio:  "<< NomParticion.str().c_str()<<endl;
              read (Leer[j],res1, sizeof(res1));            //espera la respuesta de recibi de los procesos para volver a escribir
              read (Leer[j+NumDiscos/2],res2, sizeof(res2));

              if (strcmp(res1,"NO")==0 && strcmp(res2,"NO")==0){
                cout << "Error: Archivo irreparable falta un bloque en Disco principal y en respaldo\n";
                write(Escribir[j+NumDiscos/2],"Muere",6);
                write(Escribir[j],"Muere",6);
                exit(0);
              }
              else if (strcmp(res1,"NO")==0 && strcmp(res2,"SI")==0){
                //cout << "falta en Disco "<<j<<" pero, Disco "<<j+NumDiscos/2<<" lo tieneeeeeeeeeeeeeeeeEEEEEEEEEEEEEEEEEEEEEEEEEEEEe"<<endl;
                write(Escribir[j+NumDiscos/2],"envia",6);
                write(Escribir[j],"crear",6);


                read(Leer[j+NumDiscos/2],contenido,1024);
                write(Escribir[j],contenido,1024);
                char msj[8];
                read(Leer[j],msj, sizeof(msj));
                //cout << "falta en Disco "<<j+NumDiscos/2<<" pero, Disco "<<j<<" lo tieneeeeeeeeeeeeeeeeEEEEEEEEEEEEEEEEEEEEEEEEEEEE"<<endl;
              }
              else if (strcmp(res1,"SI")==0 && strcmp(res2,"NO")==0){
                //cout << "falta en Disco "<<j+NumDiscos/2<<" pero, Disco "<<j<<" lo tieneeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee"<<endl;
                write(Escribir[j],"envia",6);
                write(Escribir[j+NumDiscos/2],"crear",6);

                read(Leer[j],contenido,1024);
                write(Escribir[j+NumDiscos/2],contenido,1024);
                char msj[8];
                read(Leer[j+NumDiscos/2],msj, sizeof(msj));
                //cout << "falta en Disco "<<j+NumDiscos/2<<" pero, Disco "<<j<<" lo tieneeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee"<<endl;
              }
              else{

                char msj[15];
                write(Escribir[j+NumDiscos/2],"NADA",5);
                write(Escribir[j],"NADA",5);

                read(Leer[j],msj,15);

                read(Leer[j+NumDiscos/2],msj,15);
                //cout <<"estoy  en SIIII SIIIIIIIII"<<endl;
              }
              //cout << " paso " << i << " Vces"<<endl;
              i++;
            } 
          }
        }

        for(int k = 0;k<NumDiscos;k++){
          write(Escribir[k],"Salir",6);
        }
        char msj[7];
        for(int k = 0;k<NumDiscos;k++){
          read(Leer[k],msj,7);
        }
        exit(0);
      }
      if(pid == 0){

        while(true){

          //cout <<  "SOy proceso "<< NumProceso<< " y eestoy esperando"<<endl;
          char NomNuevo[100];
          char NomParticion[100];
         
          read(LeerH,NomNuevo, sizeof(NomNuevo));
          //cout <<  "Nombre particion recibida "<<NomNuevo<< " En Disco "<< NumProceso<<endl;
          
          if (strcmp (NomNuevo,"Salir")==0){
            //cout << "soy proceso "<< NumProceso << "y saliiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii\n";
            write (EscribirH,"yasali",7);
            exit(0);
          }
          
          sprintf(NomParticion, "./Discos RAID/Disco %d/%s",NumProceso,NomNuevo);
          //cout << NomParticion<<endl;
          char contenido[1024];
          char mensaje[1024];
          FILE * f;             // comprueba la particion indicada
          f = fopen (NomParticion,"rb");

          if(f == NULL)
          {  
            char quehago[6];
            write(EscribirH,"NO",3);            
            read(LeerH,quehago, sizeof(quehago));            
            if(strcmp(quehago,"crear")==0) {
              FILE * fi;
              fi= fopen (NomParticion,"wb");
              read(LeerH,mensaje, 1024);
              fwrite (mensaje, 1,1024, fi);
              write(EscribirH,"Gracias",8);
              fclose (fi);
              cout << "Bloque "<< NomNuevo <<" en disco: "<<NumProceso<<", Restaurado"<<endl;
            }

          }
          else{ 
            char quehago[6];
            
            write(EscribirH,"SI",3);
            read(LeerH,quehago, sizeof(quehago));

            if (strcmp(quehago,"envia")==0){
              fread (contenido, 1,1024, f);
              write(EscribirH,contenido,1024);
            }
            else write(EscribirH,"paraqueesperes",15);
            //cout << "si lo tengo \n";
            fclose (f);
          }

          
          //cout << "sali un ratito  "<< NumProceso<<endl;
        }
      }
    }

    if(Rflag){

      int i = 0;
      if (pid == 1){


        iconfig.open("Config");
        bool check = false;
        while(!iconfig.eof()){
          string palabra;
          iconfig >> palabra;
          if (palabra.compare(NomFichero_R)==0){
            iconfig >> palabra;
            NumPartes = atoi(palabra.c_str());
            check = true;
            //cout << "el fichero "<< NomFichero_R << " tiene "<< NumPartes << "partes \n";
            //cout << "El archivo SSSSSSI esta en el sistema"<<endl;
            break;
          }
        }

        if (!check){
          cout << "Error: El archivo no esta en el sistema"<<endl;
          exit(0);
        }
        
        i=0;
        char NomOriginal[100];
        sprintf(NomOriginal, "./Archivos Originales/%s",NomFichero_R);
        FILE * original;
        original = fopen (NomOriginal,"wb");
        while(i<NumPartes){

          for(int j =0;j<NumDiscos/2;j++){
            char contenido [1024];
            if (i<NumPartes){
              
              //--------------------------- Agregar 0s al numero de fichero
              stringstream ceros;
              agregarceros(i,NumPartes,ceros,false);
              //--------------------------------
              stringstream NomParticion;
              char res1[3];
              char res2[3];
              char msj[20];
              

              NomParticion << NomFichero_R <<"-"<<ceros.str().c_str()<<i; 
              
              
              write(Escribir[j],NomParticion.str().c_str(),strlen(NomParticion.str().c_str())+1);
              write(Escribir[j + NumDiscos/2],NomParticion.str().c_str(),strlen(NomParticion.str().c_str())+1);
              read(Leer[j],res1,3);
              read(Leer[j + NumDiscos/2],res2,3);
              if (strcmp(res1,"SI")==0){

                write(Escribir[j],"Manda",6);
                read(Leer[j],contenido,1024);
                fwrite(contenido,1,1024,original);
              }else{
                if (strcmp(res2,"SI")==0){
                  write(Escribir[j+NumDiscos/2],"Manda",6);
                  read(Leer[j+NumDiscos/2],contenido,1024);
                  fwrite(contenido,1,1024,original);
                }
                else{   // no se pyede crear el fichero

                  cout << "Error: No se puede restaurar, faltan bloques en disco principal y respaldo "<<endl;
                  fclose(original);
                  stringstream comando;
                  comando << "rm ./Archivos\\ Originales/"<<NomFichero_R;
                  system(comando.str().c_str());
                  exit(0);
                }
              }
            }
            else break; 
            i++;
          }
        }



        char Murio[6];
        fclose(original);
        cout << "Archivo restaurado exitosamente, en carpeta Archivos Originales"<<endl;
        exit(0);
        

        return 0;
      }
      else{   // hijos

        while(true){

          //cout <<  "SOy proceso "<< NumProceso<< " y eestoy esperando"<<endl;
          char NomNuevo[100];
          char NomParticion[100];
         
          read(LeerH,NomNuevo, sizeof(NomNuevo));
          //cout <<  "Nombre particion recibida "<<NomNuevo<< " En Disco "<< NumProceso<<endl;
          
          
          sprintf(NomParticion, "./Discos RAID/Disco %d/%s",NumProceso,NomNuevo);
          //cout << NomParticion<<endl;
          char contenido[1024];
          char mensaje[1024];
          FILE * f;             // comprueba la particion indicada
          f = fopen (NomParticion,"rb");

          if(f == NULL)
          {  
            
            write(EscribirH,"NO",3);
            //cout << "No tengo "<< NomParticion <<" En mi disco: "<<NumProceso<<endl; 
          }
          else{ 
            char quehago[6];
            
            write(EscribirH,"SI",3);
            read(LeerH,quehago, sizeof(quehago));

            if (strcmp(quehago,"Manda")==0){
              fread (contenido, 1,1024, f);
              write(EscribirH,contenido,1024);
            }
            
            //cout << "si lo tengo \n";
            fclose (f);
          }
          
          //cout << "sali un ratito  "<< NumProceso<<endl;
        }
      }
    }

    if(mflag){
      int i = 0;
      if (pid == 1){


        iconfig.open("Config");
        bool check = false;
        string aux;
        iconfig >> aux;

        while(!iconfig.eof()){
          bool incompleto=false,irreparable=false;

          string palabra;
          string NomFichero;
          iconfig >> palabra;

          NomFichero = palabra;
          iconfig >> palabra;
          NumPartes = atoi(palabra.c_str());
          check = true;
          //cout << "el fichero "<< NomFichero << " tiene "<< NumPartes << "partes \n";
          
          i=0;
          while(i<NumPartes){
            if (irreparable)break;
            for(int j =0;j<NumDiscos/2;j++){
              char contenido [1024];
              if (i<NumPartes){
            
                //--------------------------- Agregar 0s al numero de fichero
                stringstream ceros;
                agregarceros(i,NumPartes,ceros,false);
                //--------------------------------
                stringstream NomParticion;
                char res1[3];
                char res2[3];
                char msj[20];
              

                NomParticion << NomFichero <<"-"<<ceros.str().c_str()<<i; 
              
              
                write(Escribir[j],NomParticion.str().c_str(),strlen(NomParticion.str().c_str())+1);
                write(Escribir[j + NumDiscos/2],NomParticion.str().c_str(),strlen(NomParticion.str().c_str())+1);
                read(Leer[j],res1,3);
                read(Leer[j + NumDiscos/2],res2,3);
                if (strcmp(res1,"NO")==0 && strcmp(res2,"NO")==0){
                  irreparable = true;
                  break;
                }
                else if (strcmp(res1,"SI")==0 && strcmp(res2,"NO")==0){
                  incompleto = true;
                }
                else if (strcmp(res2,"SI")==0 && strcmp(res1,"NO")==0){
                  incompleto = true;
                }
              }
              else break; 
              i++;
            }
          }
          
          if (irreparable)cout << "**"<< NomFichero<<endl;
          else if (incompleto)cout << "*"<< NomFichero<<endl;
          else cout << NomFichero<<endl;
        }
      }
      else{ // hijos 
      while(true){

        //cout <<  "SOy proceso "<< NumProceso<< " y eestoy esperando"<<endl;
        char NomNuevo[100];
        char NomParticion[100];
       
        read(LeerH,NomNuevo, sizeof(NomNuevo));
        //cout <<  "Nombre particion recibida "<<NomNuevo<< " En Disco "<< NumProceso<<endl;
        
        if (strcmp (NomNuevo,"Salir")==0){
          //cout << "soy proceso "<< NumProceso << "y saliiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii\n";
          write (EscribirH,"muero",6);
          exit(0);
        }
        
        sprintf(NomParticion, "./Discos RAID/Disco %d/%s",NumProceso,NomNuevo);
        //cout << NomParticion<<endl;
        char contenido[1024];
        char mensaje[1024];
        FILE * f;             // comprueba la particion indicada
        f = fopen (NomParticion,"rb");

        if(f == NULL)
        {  
          
          write(EscribirH,"NO",3);
          //cout << "No tengo "<< NomParticion <<" En mi disco: "<<NumProceso<<endl; 
        }
        else{ 
        
          write(EscribirH,"SI",3);
          fclose (f);
        }
        
        //cout << "sali un ratito  "<< NumProceso<<endl;
      }
      }  
    }
  }
  else if ((Eflag || gflag || eflag || rflag || Rflag || mflag)&& f == NULL){
    cout << "Error: Se necesita fichero Config para realizar la acción"<<endl;
    exit(0);
  }
 return 0;
}