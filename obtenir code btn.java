
string get_code (btnCode){
  if(identifiantTelecommande == 0xFF00) {  
 
      // Switch basé sur le code du bouton.
      switch(codeBouton) {
    
        // Si le code bouton est 0xBA45
        case 0xBA45:
          // C'est le bouton "power" qui a été pressé.
          Serial.println("Bouton power");
          break;
          
        case 0xB946:
          Serial.println("Bouton Vol+");
          break;
          
        case 0xB847:
          Serial.println("Bouton Func/Stop");
          break;
          
        case 0xBB44:
          Serial.println("Bouton |<<");
          break;
          
        case 0xBF40:
          Serial.println("Bouton >||");
          break;
    
        case 0xBC43:
          Serial.println("Bouton >>|");
          break;
          
        case 0xF807:
          Serial.println("Bouton v");
          break;
    
        case 0xEA15:
          Serial.println("Bouton Vol-");
          break;
          
        case 0xF609:
          Serial.println("Bouton ^");
          break;
          
        case 0xE916:
          Serial.println("Bouton 0");
          break;
          
        case 0xE619:
          Serial.println("Bouton EQ");
          break;
          
        case 0xF20D:
          Serial.println("Bouton ST/REPT");
          break;
          
        case 0xF30C:
          Serial.println("Bouton 1");
          break;
          
        case 0xE718:
          Serial.println("Bouton 2");
          break;
          
        case 0xA15E:
          Serial.println("Bouton 3");
          break;
          
        case 0xF708:
          Serial.println("Bouton 4");
          break;
          
        case 0xE31C:
          Serial.println("Bouton 5");
          break;
          
        case 0xA55A:
          Serial.println("Bouton 6");
          break;
          
        case 0xBD42:
          Serial.println("Bouton 7");
          break;
          
        case 0xAD52:
          Serial.println("Bouton 8");
          break;
          
        case 0xB54A:
          Serial.println("Bouton 9");
          break;
    
        // Si le code bouton n'est pas valide.
        default:
          Serial.println("Bouton inconnu ou erreur de transmission");
          break;
      }

   }
   else {

        // Note : c'est une télécommande qui n'est pas supportée ou il y a une erreur de transmission.
       
       Serial.println("Télécommande non supportée ou erreur de transmission.");
    
   }

   Serial.println("");
   
}
