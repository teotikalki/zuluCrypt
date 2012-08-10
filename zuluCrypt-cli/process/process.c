/*
 * 
 *  Copyright (c) 2012
 *  name : mhogo mchungu 
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */ 

#include "process.h"

struct Process_t{
	size_t len ;
	pid_t pid ;
	int pd[2] ;
	int state ;
	int std_io ;
	char * exe ;
	char delimiter ;
	char ** args ;
	int args_source ;
	int signal ;
	int timeout ;
	int wait_status ;
	uid_t uid ;
	pthread_t * thread ;
};

void ProcessSetArgumentList( process_t p,... )
{
	if( p == NULL )
		return ;
	
	char * entry ;
	char ** args  ;
	size_t size = sizeof( char * ) ;
	int index = 0 ;
	va_list list ;
	
	args = ( char ** )malloc( size ) ;
	args[ index ] = p->exe ;
	index++ ;
	
	va_start( list,p ) ;
	
	while( 1 ){
		entry = va_arg( list,char * ) ;
		args = ( char ** )realloc( args,( 1 + index ) * size ) ;
		args[ index ] = entry ;
		index++ ;
		if( entry == '\0' )
			break ;
	}
	
	va_end( list ) ;
	
	p->args = args ;
	p->args_source = 0 ;	
}

static void ProcessSetArguments_1( process_t p ) 
{
	/*
	 * this function converts a one dimentional array into a two dimentional array as expected by the second argument of execl
	 * 
	 */
	size_t k = 0 ;
	
	char * c ;
	const char * d ;
	
	char ** f ;
	char delimiter ; 
	
	if( p->args != NULL ){
		/*
		 * Assuming the arments list was set by ProcessSetArguments()
		 */
		return ;		
	}
	
	p->args_source = 0 ;

	delimiter = p->delimiter ;
	
	d = p->exe - 1 ;
	
	/*
	 * find out to how many pieces we should break the string
	 */
	while( *++d ){
		if( *d == delimiter ) ;
		k++ ;
	}
	
	/*
	 * create an array of pointers,each slot will point to each "piece" of string creating a 2-D array
	 */
	f = ( char ** ) malloc( sizeof( char * ) * ( k + 2 ) ) ;

	*f = p->exe ;
	
	c = p->exe - 1;

	k = 1 ;
	
	while( *++c ){
		if( *c == delimiter ){
			*c = '\0' ;      /* add null to break a string into pieces */
			f[ k ] = c + 1 ; /* point to the beginning of the next piece   */
			k++ ;
		}
	}
	
	f[ k ] = '\0' ;
	
	p->args = f ;
}

static void * __timer( void * x )
{
	process_t  p = ( process_t ) x ;
	
	sleep( p->timeout ) ;
	
	kill( p->pid,p->signal ) ;
	
	p->state = CANCELLED ;
	
	return ( void * ) 0 ; 
}

static void __ProcessStartTimer( process_t p )
{
	p->thread = ( pthread_t * ) malloc( sizeof( pthread_t ) ) ;
	
	if( p->thread == NULL )
		return  ;
		
	pthread_create( p->thread,NULL,__timer,( void * ) p );	
}

pid_t ProcessStart( process_t p ) 
{
	if( p == NULL )
		return -1 ;
	
	if( p->std_io >= 4 )
		if( pipe( p->pd ) != 0 ) 
			return -1 ;

	p->pid = fork() ;
	
	if( p->pid == -1 )
		return -1 ;
	
	ProcessSetArguments_1( p ) ;
	
	if( p->pid == 0 ){
				
		if( p->uid != -1 ){
			setuid( p->uid ) ;			
			seteuid( p->uid ) ;
		}
		
		/*
		#define CLOSE_STD_OUT 		1
		#define CLOSE_STD_ERROR		2
		#define CLOSE_BOTH_STD_OUT 	3
		#define READ_STD_OUT 		4
						5 = 4 + 1
						6 = 4 + 2
						7 = 4 + 3
		#define READ_STD_ERROR 		8
						9 = 8 + 1
						..
						..
		#define WRITE_STD_IN 		12
						13 = 12 + 1
						..
						..
		*/
		
		switch( p->std_io ){
			case 1     : close( 1 ) ; 
				     break ;
			case 2     : close( 2 ) ; 
				     break ;
			case 3     : close( 1 ) ;
				     close( 2 ) ; 
				     break ;
			
			case 4     :	     
			case 5     : close( 1 ) ;
				     close( p->pd[ 0 ] ) ;
				     dup2( p->pd[ 1 ],1 ) ;
				     break ;
				
			case 6     :	     
			case 7     : close( 2 ) ;
				     close( 1 ) ;
			             close( p->pd[ 0 ] ) ;
			             dup2( p->pd[ 1 ],1 ) ;
			             break ;				     
			case 10    :	     
			case 8     : close( 2 ) ;
				     close( p->pd[ 0 ] ) ;
			             dup2( p->pd[ 1 ],2 ) ;
			             break ;	
			case 11    :	     
			case 9     : close( 1 ) ;
				     close( 2 ) ;
				     close( p->pd[ 0 ] ) ;
				     dup2( p->pd[ 1 ],2 ) ;
			       	     break ;

			case 12    : close( 0 ) ;
				     close( p->pd[ 1 ] ) ;
			             dup2( p->pd[ 0 ],0 ) ;
				     break ;
			case 13    : close( 1 ) ;
				     close( 0 ) ;
				     close( p->pd[ 1 ] ) ;
				     dup2( p->pd[ 0 ],0 ) ;
				     break ;
			case 14    : close( 1 ) ;
				     close( 0 ) ;
			             close( p->pd[ 1 ] ) ;
			             dup2( p->pd[ 0 ],0 ) ;
			             break ;	     
		}
				
		execv( p->args[0],p->args ) ;
		/*
		 * execv has failed :-( 
		 */
		_Exit( 1 ) ;
		/*
		 * child process block ends here
		 */
	}
		
	p->state = RUNNING ;
		
	if( p->timeout != -1 )
		__ProcessStartTimer( p ) ;
	
	/*
	 * parent process continues from here
	 */
	if( p->std_io <= 3 )
		;
	else if( p->std_io < 12 )
		close( p->pd[ 1 ] ) ;
	else
		close( p->pd[ 0 ] ) ;
	
	return p->pid ;
}

char * ProcessGetOutPut( process_t p ) 
{
	#define SIZE 64
	char * buffer = NULL ;
	char buff[ SIZE ] ;
	int size = 0 ;
	int count ;

	if( p == NULL )
		return NULL ;
	
	while( 1 ) {
		count = read( p->pd[ 0 ],buff,SIZE ) ;
		if( count == SIZE ){
			buffer = ( char * ) realloc( buffer,size + SIZE + 1 ) ;
			memcpy( buffer + size,buff,SIZE ) ;
			buffer[ size + SIZE ] = '\0' ;
			size += SIZE ;
		}else if( count < SIZE && count != 0 ){
			buffer = ( char * ) realloc( buffer,size + count + 1 ) ;
			memcpy( buffer + size,buff,count ) ;
			buffer[ size + count ] = '\0' ;
			size += count ;
		}else
			break ;	
	}	
	
	return buffer ;
}

int ProcessState( process_t p ) 
{
	if( p != NULL )
		return p->state ;
	else
		return -1 ;
}

int ProcessGetOutPut_1( process_t p,char * buffer,int size ) 
{
	if( p != NULL )		
		return read( p->pd[ 0 ],buffer,size ) ;
	else
		return -1 ;
}

int ProcessWrite( process_t p,const char * data ) 
{	
	if( p != NULL )
		return write( p->pd[ 1 ],data,strlen( data ) ) ;
	else
		return -1 ;
}

process_t Process( const char * path ) 
{
	if( path == NULL )
		return NULL;
	
	process_t p = ( process_t ) malloc( sizeof( struct Process_t ) ) ;
	
	if( p == NULL )
		return NULL ;
	
	p->len = strlen( path ) ;
	
	p->exe = ( char * ) malloc( sizeof( char ) * ( p->len + 1 ) ) ;

	if( p->exe == NULL ){
		free( p ) ;
		return NULL ;
	}
	
	strcpy( p->exe,path ) ;
	
	p->std_io = 0 ;
	p->delimiter = ' '  ;
	p->args = NULL      ;
	p->state = HAS_NOT_START;
	p->signal = -1 ;
	p->timeout = -1 ;
	p->wait_status = -1 ;
	p->args_source = -1 ;
	p->uid = -1 ;
	p->thread = NULL ;
	
	return p ;
}

void ProcessSetOption( process_t p,int opt ) 
{
	if( p != NULL )
		p->std_io += opt ;	
}

void ProcessSetOptionTimeout( process_t p,int timeout,int signal ) 
{
	if( p == NULL )
		return ;
	
	p->signal = signal ;
	p->timeout = timeout ;
}

void ProcessSetOptionDelimiter( process_t p,char s ) 
{
	if( p != NULL )
		p->delimiter = s ;
}

void ProcessDelete( process_t * p ) 
{
	if( p == NULL )
		return ;
	
	process_t px = *p ;
	*p = NULL ;

	if( px->thread != NULL ){
		pthread_cancel( *(px)->thread ) ;
		free( px->thread ) ;
	}	
	
	if( px->std_io <= 3 )
		;
	else if( px->std_io < 12 )
		close( px->pd[ 0 ] ) ;
	else
		close( px->pd[ 1 ] ) ;	
	
	if( px->wait_status == -1 )
		waitpid( px->pid,0,WNOHANG ) ;
		
	if( px->args != NULL )
		if( px->args_source == 0 )
			free( px->args ) ;
	
	if( px->exe != NULL )
		free( px->exe ) ;
	
	free( px ) ;		
}

int ProcessTerminate( process_t p ) 
{
	int st ;
	
	if( p == NULL )
		return -1;
	
	p->state = CANCELLED ;
	st = kill( p->pid,SIGTERM ) ;
	waitpid( p->pid,0,WNOHANG ) ;
	
	return st ;
}

void ProcessSetUser( process_t p,uid_t uid ) 
{
	if( p != NULL )
		p->uid = uid ;
}

int ProcessKill( process_t p ) 
{
	int st ;
	
	if( p == NULL )
		return -1;
	
	p->state = CANCELLED ;
	
	st = kill( p->pid,SIGKILL ) ;
	waitpid( p->pid,0,WNOHANG ) ;
	
	return st ;	
}

int ProcessExitStatus( process_t p )
{
	int status ;
	
	if( p == NULL )
		return -1;
	
	waitpid( p->pid,&status,0 ) ;	
	p->state = FINISHED ;		
	p->wait_status = 1 ;	
	return status ;
}

void ProcessSetArguments( process_t p,char * const s[] ) 
{
	if( p == NULL )
		return ;
	
	p->args = ( char ** ) s ;
	p->args_source = 1 ;
}

int ProcessSetSearchPaths( const char * s ) 
{
	return -1 ;
}
