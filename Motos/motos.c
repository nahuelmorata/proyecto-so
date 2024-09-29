

/*
 *
 * Te dejo una ayuda para que empices marcos asi vas mas rapido
 *
 * Estos son los semaforos que necesitas
 * ruedas,cuadro,motor,pintor,extra
 *
 * rueda comieza en 2 el resto en 0
 *
 *
 *
 */

void ruedas(){

    //Hacaer rueda

    signal(cuadro);
    wait(ruedas);
}

void cuadro(){
    wait (cuadro);
    wait(cuadro);

    signal(motor);
}

void motor(){
    wait(motor);

    signal(pintar);
}


void pintor(){
    int i=0;
    wait(pintar)

    if(i==1){
      signal(extra);

        i--;
    }else{
        i++
        //Terminar moto
    signal(ruedas);
    signal(ruedas);
    }
}



void extra(){
    wait(extra)

    //Terminar moto
    signal(ruedas);
    signal(ruedas);
}
