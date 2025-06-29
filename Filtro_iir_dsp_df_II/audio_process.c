/*
 *   audio_process.c
 */

//* Standard Linux headers **
#include     <stdio.h>		// Always include stdio.h
#include     <stdlib.h>		// Always include stdlib.h
#include     <string.h>		// Defines memcpy

//* Application headers **
#if defined(_TMS320C6X)		// Here's how to test for the DSP
#elif defined(__GNUC__)		// Test for the ARM
//#include     "debug.h"	// DBG and ERR macros
#endif

#include    "audio_process.h"

int audio_process(short *outputBuffer, short *inputBuffer, int samples,short *state) {

    int n;
    short *Coefs; // Puntero a los coeficientes
    int nCoefs;

    int j, x, t_0, t_1, p_0, p_1, p_2, p_3;

    int temp_1, temp_2;

    int s_32, s_10;

    long long c_7654;
    long long c_3210;
    long long s_3210;

//filtro pasa-bajo butterworth  FC=1k
#define FILTER_LEN_MA   8
short NUM[ FILTER_LEN_MA] = {
32767,  16384,
30942, -14825,
32767,  16384,
28992, -12858
};//Ganancia 1ero=67 2do=63

//filtro pasa-alto butterworth  FC=1k
/*#define FILTER_LEN_MA   8
short NUM[ FILTER_LEN_MA] = {
-32768,  16384,
30942,  -14825,
-32768,  16384,
28992,  -12858
};//Ganancia 1ero=15538 2do=14558
*/

/*//filtro pasa-banda butterworth  FC=3k a 6k
#define FILTER_LEN_MA   8
short NUM[ FILTER_LEN_MA] = {
0, -16384,
26897,  -13221,
0, -16384,
21081,  -11656
};//Ganancia 1ero=2836 2do=2836
*/

/*
//filtro pasa-banda butterworth  FC=1k a 2k
#define FILTER_LEN_MA   8
short NUM[ FILTER_LEN_MA] = {
0, -16384,
30101,  -14598,
0, -16384,
31340,  -15281
};//Ganancia 1ero=1025 2do=1025
*/

/*//filtro Elimina-banda butterworth  FC=3k a 6k
#define FILTER_LEN_MA   8
short NUM[ FILTER_LEN_MA] = {
-27779,  16384,
26897,  -13221,
-27779,  16384,
21081,  -11656
};//Ganancia 1ero=14256 2do=14256
*/
	Coefs=(short *)NUM;
	nCoefs=FILTER_LEN_MA;

for ( n = 1; n < samples; n+=2 ){

//Codigo de DSP C intrinsico

    /* -------------------------------------------------------------------- */
    /*  Inicializa nuestra entrada                                          */
    /* -------------------------------------------------------------------- */
    x = inputBuffer[n];

    /* -------------------------------------------------------------------- */
    /* Informa al compilador acerca de las alineaciones.                    */
    /* -------------------------------------------------------------------- */
    _nassert((int)Coefs % 8 == 0);
    _nassert((int)state % 8 == 0);
    _nassert((int)nCoefs % 4 == 0);
    _nassert((int)nCoefs >= 4);

    /* -------------------------------------------------------------------- */
    /* Iteración sobre los biquads, procesando dos por iteración.           */
    /* -------------------------------------------------------------------- */
     for (j = 0; j < nCoefs; j += 8) {

        c_3210 = _mem8_const(&Coefs[j]);
        s_3210 = _mem8(&state[j]);
        c_7654 = _mem8_const(&Coefs[j + 4]);

	s_10 = _loll(s_3210);
        s_32 = _hill(s_3210);

	p_0 = _dotp2(_hill(c_3210), s_10);
        p_1 = _dotp2(_loll(c_3210), s_10);

        p_2 = _dotp2(_hill(c_7654), s_32);
        p_3 = _dotp2(_loll(c_7654), s_32);

	x = (x*67); //Ganancia del primer biquad
	x>>=15;

	t_0 = (x + (p_0 >> 14));
        x += ((p_0 + p_1) >> 14);

	x=(x*63); //Ganancia del segundo biquad

	x>>=14;

        t_1 = (x + (p_2 >> 14));
        x += ((p_2 + p_3) >> 14);

	x=x<<1;
	/*-----------------------------------------------------------*/

	temp_2 = _pack2(s_32,t_1);
        temp_1 = _pack2(s_10,t_0);

        if (j < nCoefs - 4)
        {
            _mem8(&state[j]) = _itoll(temp_2, temp_1);
        }
        else
        {
            state[j] = _pack2(s_10,t_0);
        }
    }

  outputBuffer[n]=x;
  outputBuffer[n-1]=inputBuffer[n];

  }
   return 0;
}
