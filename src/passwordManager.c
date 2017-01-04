/*
 Copyright (c) 2012-2017 Kirill Belyaev
 * kirillbelyaev@yahoo.com
 * kirill@cs.colostate.edu
 * TeleScope - XML Message Stream Broker/Replicator Platform
 * This work is licensed under the Creative Commons Attribution-NonCommercial 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc/3.0/ or send 
 * a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

/*
 "DION
 The violent carriage of it
 Will clear or end the business: when the oracle,
 Thus by Apollo's great divine seal'd up,
 Shall the contents discover, something rare
 Even then will rush to knowledge. Go: fresh horses!
 And gracious be the issue!"

 Winter's Tale, Act 3, Scene 1. William Shakespeare
 */

#include "passwordManager.h"

char * pwFile = (char *)"telescope_pass";

unsigned long seed[2];
char *password;
int i;

bool authenticateUser( int sock )
{
  const char * const pwPrompt = "Password: ";
  char salt[12] = "$1$........";
  char * readBuffer = NULL;
  size_t maxReadSize = 0;
  char recvBuffer[MIN_LINE];

  FILE * fd;

  // Get password response
  write( sock, pwPrompt, strlen(pwPrompt) );
  if ( receivedInBuffer( recvBuffer, sock, MIN_LINE-1 ) <= 0 )
    return false;

  if ( ! access( pwFile, R_OK ) )
  {

    // Open pwFile
    if ( ( fd = fopen( pwFile, "r" ) ) < 0 )
    {
      perror( "fopen password file failed" );
      exit(-1);
    }

    // Get the pw hash
    if ( getline( &readBuffer, &maxReadSize, fd ) < 11 )
    {
      logMessage( stderr, "Invalid data in password file\n" );
      free( readBuffer );
      return false;
    }
    trim( readBuffer );

    // Copy the salt
    strncpy( salt, readBuffer, 11 );

    // encrypt the user provided password using the salt
    char * result = crypt(recvBuffer, salt);

    bool retval = ( strcmp( result, readBuffer ) == 0 );

    free( readBuffer );

    return retval;
  }
  else
  {
    return ( strcmp( DEFAULT_ACCESS_PASSWORD, recvBuffer ) == 0 );
  }
}

void setNewPassword( int sock )
{
  const char *const saltChars = "./0123456789ABCDEFGHIJKLMNOPQRST"
                                "UVWXYZabcdefghijklmnopqrstuvwxyz";
  const char * const pwPrompt = "Current ";
  const char * const newPwPrompt = "New Password: ";
  const char * const confirmPwPrompt = "Repeat New Password: ";
  FILE * fd;
  char salt[12] = "$6$........";
  char newPw[MIN_LINE], confirmPw[MIN_LINE];
  srand(time(NULL));

  // Confirm current user password
  write( sock, pwPrompt, strlen(pwPrompt) );
  if ( ! authenticateUser( sock ) )
    return;

  // Get new user password
  write( sock, newPwPrompt, strlen(newPwPrompt) );
  if ( receivedInBuffer( newPw, sock, MIN_LINE-1 ) <= 0 )
    return;
  write( sock, confirmPwPrompt, strlen(confirmPwPrompt) );
  if ( receivedInBuffer( confirmPw, sock, MIN_LINE-1 ) <= 0 )
    return;

  if ( strcmp( newPw, confirmPw ) != 0 )
  {
      logMessage( stderr, "Password does not match.\n" );
      return;
  }
  
  // Open password file for writing
  if ( ( fd = fopen( pwFile, "w" ) ) < 0 )
  {
      perror( "fopen password file failed" );
      exit(-1);
  }

  // Generate salt
  for ( i = 3; i < 11; i++ )
    salt[i] = saltChars[ rand() % strlen( saltChars ) ];

  // Generate encrypted pw
  char * result = crypt(newPw, salt);

  // Output password file and close
  fprintf( fd, "%s\n", result );
  fclose( fd );
  return;
}
