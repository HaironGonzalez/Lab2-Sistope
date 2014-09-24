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
  opterr = 0;

  while ((c = getopt (argc, argv, "C:g:e:r:R:mE")) != -1)
  {
    switch (c)
    {
      case 'C':
        Cflag = true;
        aux = optarg;
        NumDiscos= atoi(aux);
        break; 
      case 'E':
        Eflag = true;
        break;
      case 'g':
        gflag = true;
        NomFichero_g = optarg;
        break;
      case 'e':
        eflag = true;
        NomFichero_e = optarg;
        break;
      case 'r':
        rflag = true;
        NomFichero_r = optarg;
        break;
      case 'R':
        Rflag = true;
        NomFichero_R = optarg;
        break;
      case 'm':
        mflag = true;
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

  int pid;
  int NumPipes = NumDiscos * 2;
  int fd [NumPipes][2];
  int NumProceso;
  double largo;
  int NumPartes;

    for(int i=0;i<NumPipes;i++)
    {
      if (pipe(fd[i]) == -1)
      {
          perror("No se pudo crear el pipe\n");
          exit(1);
      }
    }

  FILE *conf;
  conf = fopen ("Config","r");
  if (conf != NULL){
    // manejar fichero config muy importante
  }
  else if (Cflag){
    
    
    for (int i=0;i<NumDiscos;i++){

      pid = fork();
      if(pid == 0){
        close(fd[2*i][0]);    // 0 entrada 1 salida
        close(fd[2*i+1][1]);
        NumProceso = i;
        //cout << "soy hijo "<<i<<endl;

        break;
      }
      else if(i == NumDiscos-1){
        for (int j=0;j<NumDiscos;j++){

        close(fd[2*j][1]);
        close(fd[2*j+1][0]);
        }
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
      write(fd[2*NumProceso][1],msg.str().c_str(), strlen(msg.str().c_str())+1);
    }
    else if(pid == 1){

      for (int i=0;i<NumDiscos;i++){   // espera a que todos los procesos creen su Disco 
        read(fd[2*i][0],hijoapapi, sizeof(hijoapapi));
        //cout << hijoapapi <<endl;
      }
    }   
  }

  

  if (gflag){

    
    if(pid==0){

      //cout << "Proceso "<<NumProceso<<" Activo"<<endl;
      while(true){


        char mensaje[1024];
        char NomNuevo[100];
        char NomParticion[100];
        read(fd[2*NumProceso+1][0],NomNuevo, sizeof(NomNuevo));
        //cout << "Nombre particion "<<NomNuevo<< "En Disco "<< NumProceso;
        
        if (strcmp (NomNuevo,"Salir")==0){
          //cout << "soy proceso "<< NumProceso << "y sali\n";
          break;
        }
        sprintf(NomParticion, "./Discos RAID/Disco %d/%s",NumProceso,NomNuevo);
        //cout << NomParticion<<endl;    
        write(fd[2*NumProceso][1],"recibi",7); 

        read(fd[2*NumProceso+1][0],mensaje, sizeof(mensaje));

        FILE *in;                                  // crea el particion indicada
        in = fopen (NomParticion,"wb");
        if(in==NULL)
        {
          perror("No se pudo crear la partición\n");
          exit(1);
        }
        fwrite (mensaje, 1,1024, in);
        fclose (in);
      }
    }
    else{
      
      char nombre[200];
      
      sprintf(nombre, "./Archivos Originales/%s",NomFichero_g);
      //cout << "mandosd"<<NomFichero_g<<endl;
      FILE *f;
      f = fopen (nombre,"rb");
      if(f==NULL){
      perror("No se puede abrir fichero");
      }
      fseek(f, 0L, SEEK_END);
      largo = ftell(f);
      cout << "el largo es "<<largo<<endl;
      fclose(f);
      int i=0;
      
      f = fopen (nombre,"rb");
      int cont = 0;
      char hijoapapi[100];
      while(i<largo){

        for(int j =0;j<NumDiscos/2;j++){
          
          if (i<largo){
            
            //--------------------------- Agregar 0s al numero de fichero
            int aux1=largo/1024;
            int aux2=cont;
            int digitoslargo=1;
            int digitoscont=1;
            int cantidadceros=0;
            stringstream ceros;
            while(aux1 != 0){
              aux1 = aux1/10;
              if(aux1 > 0) digitoslargo++;
            }
            while(aux2 != 0){
              aux2 = aux2/10;
              if(aux2 > 0) digitoscont++;
            }
            cantidadceros=digitoslargo-digitoscont;
            if(cantidadceros > 0){

              for(int k=0;k<cantidadceros;k++){
                ceros << "0";
              }
            }
            //--------------------------------
            char contenido[1024];
            fread (contenido, 1, 1024, f);
            stringstream NomParticion;
            
            NomParticion << NomFichero_g <<"-"<<ceros.str().c_str()<<cont;
            
            write(fd[2*j+1][1],NomParticion.str().c_str(),strlen(NomParticion.str().c_str())+1);
            write(fd[2*j+1+NumDiscos][1],NomParticion.str().c_str(),strlen(NomParticion.str().c_str())+1);
            //cout << contenido;
            //cout << "mando "<<NomParticion.str().c_str()<<endl;
            read (fd[2*j][0],hijoapapi, sizeof(hijoapapi));            //espera la respuesta de recibi de los procesos para volver a escribir
            read (fd[2*j+NumDiscos][0],hijoapapi, sizeof(hijoapapi));
            //cout <<hijoapapi;

            write(fd[2*j+1][1],contenido,1024);
            write(fd[2*j+1+NumDiscos][1],contenido,1024); 
            i = i + 1024;
            cont++;
          }
          else break;
        }
      }
      for(int i = 0;i<NumDiscos;i++){
        write(fd[2*i+1][1],"Salir",6);
      }
      cout<< "Mando "<<cont << "particiones"<<endl;
      NumPartes = cont;
    }
  }


  if (eflag){

    if(pid == 0)
    {
      char NomNuevo[100];
      char NomaEliminar[100];
      read(fd[2*NumProceso+1][0],NomNuevo, sizeof(NomNuevo));
      cout << "Mi papi dijo "<< NomNuevo<<endl;
      //cout << "Nombre particion "<<NomNuevo<< "En Disco "<< NumProceso;
      sprintf(NomaEliminar, "./Discos RAID/Disco %d/%s",NumProceso,NomNuevo);
      
      FILE *f;
      f = fopen (NomaEliminar,"rb");
      if(f==NULL){
        perror("NOO tengo ese archivo");
      }
      else{
        fclose(f);
        stringstream comando;
        comando << "rm ./Discos\\ RAID/Disco\\ "<<NumProceso <<"/"<<NomNuevo;
        system(comando.str().c_str());
        cout << "el comando fue "<<comando.str().c_str()<<endl;
      }

    }
    else
    { 
      srand(time(0));
      int aleatorio = rand() % NumPartes;
      srand(time(0));
      int OriginaloRespaldo = rand() % 2 + 1;
      stringstream ParEliminar;
      //--------------------------- Agregar 0s al numero de fichero
      int aux1=largo/1024;
      int aux2=aleatorio;
      int digitoslargo=1;
      int digitosaleatorio=1;
      int cantidadceros=0;
      stringstream ceros;
      while(aux1 != 0){
        aux1 = aux1/10;
        if(aux1 > 0) digitoslargo++;
      }
      while(aux2 != 0){
        aux2 = aux2/10;
        if(aux2 > 0) digitosaleatorio++;
      }
      cantidadceros=digitoslargo-digitosaleatorio;
      if(cantidadceros > 0){

        for(int k=0;k<cantidadceros;k++){
          ceros << "0";
        }
      }
      //--------------------------------

      int i;
      ParEliminar << NomFichero_e <<"-"<<ceros.str().c_str()<<aleatorio;
      cout << "hay que eliminar "<< ParEliminar.str().c_str()<<endl;
      if (OriginaloRespaldo == 1){

        for (i=0;i<NumDiscos/2;i++){    
          write(fd[2*i+1][1],ParEliminar.str().c_str(),strlen(ParEliminar.str().c_str())+1);
        }
      }
      else
      {
        for (i=0;i<NumDiscos/2;i++)
        {    
          write(fd[2*i+1+NumDiscos][1],ParEliminar.str().c_str(),strlen(ParEliminar.str().c_str())+1);
        }
      }
    }
  }
  if(Eflag){

    system ("rm -R ./Discos\\ RAID/Disco*");
    //system ("rm ./Config");
  }


 return 0;
}