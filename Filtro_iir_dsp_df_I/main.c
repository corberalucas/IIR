/*
 * main.c
 *
 * ============================================================================
 * Copyright (c) UTN FRH
 *
 * Use of this software is controlled by the terms and conditions found in the
 * license agreement under which this software has been supplied or provided.
 * ============================================================================
 */

// Standard Linux headers
#include     <stdio.h>              
#include     <stdlib.h>             
#include     <signal.h>             // Señales (Ctrl-C)


// Application headers
#include     "debug.h"
#include     "audio_thread.h"

// Global audio thread environment
audio_thread_env audio_env = {0};

/* Store previous signal handler and call it */
void (*pSigPrev)(int sig);

// Callback called when SIGINT is sent to the process (Ctrl-C)
void signal_handler(int sig)
{
    DBG( "Ctrl-C Presionado, Saliendo..\n" );
    audio_env.quit = 1;

    if( pSigPrev != NULL )
        (*pSigPrev)( sig );
}


//*****************************************************************************
//*  main
//*****************************************************************************
int main( int argc, char *argv[] )
{
    int   status = EXIT_SUCCESS;

    void *audioThreadReturn;


    // Set the signal callback for Ctrl-C
    pSigPrev = signal(SIGINT, signal_handler);

    // Call audio thread function
    audioThreadReturn = audio_thread_fxn( (void *) &audio_env );

    if( audioThreadReturn == AUDIO_THREAD_FAILURE )
    {
        DBG( "Audio thread saliendo con FAILURE status\n" );
        status = EXIT_FAILURE;
    }
    else
        DBG( "Audio thread saliendo con SUCCESS status\n" );

    exit( status );
}

