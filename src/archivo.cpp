#include "bibliotecas.h"

void agregarceros (int numero,int largo,stringstream &ceros){

    int aux1=largo/1024;
    int aux2=numero;
    int digitoslargo=1;
    int digitosnumero=1;
    int cantidadceros=0;
    while(aux1 != 0){
      aux1 = aux1/10;
      if(aux1 > 0) digitoslargo++;
    }
    while(aux2 != 0){
      aux2 = aux2/10;
      if(aux2 > 0) digitosnumero++;
    }
    cantidadceros=digitoslargo-digitosnumero;
    if(cantidadceros > 0){

      for(int k=0;k<cantidadceros;k++){
        ceros << "0";
      }
    }
}