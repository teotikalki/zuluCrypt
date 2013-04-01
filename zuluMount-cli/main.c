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

#include "includes.h"
#include <signal.h>

/*
 * below 4 functions are defined in ./print_mounted_volumes.c
 */
int zuluMountPrintMountedVolumes( uid_t uid ) ;
void zuluMountPrintDeviceProperties( const char * entry ) ;
void zuluMountPrintDeviceProperties_1( string_t,uid_t ) ;

/*
 * All functions with "EXE" in their names are defined somewhere in ../zuluCrypt-cli/bin 
 */

static int _mount_get_opts( int argc,char * argv[],ARGS * args ) 
{	
	int c ;
	while( ( c = getopt( argc,argv,"HcMLntSshlPmuDd:z:e:y:p:f:G:" ) ) != -1 ) {
		switch( c ){
			case 'M' : args->share   = 1      ; break ;
			case 'n' : args->mpo     = 1      ; break ;
			case 'D' : args->action  = "-D"   ; break ;
			case 't' : args->action  = "-t"   ; break ;
			case 's' : args->action  = "-s"   ; break ;
			case 'S' : args->action  = "-S"   ; break ;
			case 'l' : args->action  = "-l"   ; break ;
			case 'L' : args->action  = "-L"   ; break ;
			case 'P' : args->action  = "-P"   ; break ;
			case 'm' : args->action  = "-m"   ; break ;
			case 'u' : args->action  = "-u"   ; break ;
			case 'c' : args->action  = "-c"   ; break ;
			case 'd' : args->device  = optarg ; break ;
			case 'z' : args->m_point = optarg ; break ;
			case 'e' : args->m_opts  = optarg ; break ;
			case 'y' : args->fs_opts = optarg ; break ;
			case 'p' : args->key     = optarg ; 
				   args->key_source = "-p"; break ;
			case 'f' : args->key     = optarg ;
				   args->key_source = "-f"; break ;
			case 'G' : args->key     = optarg ;
				   args->key_source = "-G"; break ;
			case 'H' : args->tcrypt_hidden_volume  = 1;  
				   break ;
			default  : return -1 ;
		}
	}
	
	return 0 ;
}

int _zuluExit( int st,string_t z,char * q,const char * msg )
{
	if( q != NULL ){
		free( q ) ;
	}
	StringDelete( &z ) ;	
	if( msg != NULL ){
		printf( "%s\n",msg ) ;
	}
	return st ;
}

static int _zuluExit_2( int st,stringList_t z,stringList_t q,const char * msg )
{
	StringListDelete( &q ) ;
	StringListDelete( &z ) ;
	
	if( msg != NULL ){
		printf( "%s\n",msg ) ;
	}
	return st ;
}

int _zuluExit_1( int st,stringList_t z,char * q,const char * msg )
{
	if( q != NULL ){
		free( q ) ;
	}
	StringListDelete( &z ) ;
	if( msg != NULL ){
		printf( "%s\n",msg ) ;
	}
	return st ;
}

static int _zuluMountDeviceList( uid_t uid )
{
	if( uid ){;}
	/*
	 * zuluCryptPrintPartitions() is defined in ../zuluCrypt-cli/partitions.c
	 * 
	 * it printf() contents of "/proc/partitions" 
	 * ZULUCRYPTallPartitions is set in ../zuluCrypt-cli/constants.h
	 * 
	 */
	return zuluCryptPrintPartitions( ZULUCRYPTallPartitions,0 ) ;
}

static int _zuluMountMountedList( uid_t uid )
{
	/*
	 * zuluMountPrintMountedVolumes() is defined in ./process_mountinfo.c
	 */
	return zuluMountPrintMountedVolumes( uid ) ;
}

int zuluMountVolumeStatus( const char * device,const char * UUID,uid_t uid )
{
	char * dev = NULL ;
	int st ;
	string_t p ;
	const char * e ;
	
	if( UUID == NULL ){
		if( StringPrefixEqual( device,"/dev/loop" ) ){
			/*
			* zuluCryptLoopDeviceAddress_1() is defined in ../zuluCrypt-cli/lib/create_loop_device.c
			*/
			dev = zuluCryptLoopDeviceAddress_1( device ) ;
			if( dev != NULL ){
				st = zuluCryptEXEVolumeInfo( strrchr( dev,'/' ) + 1,dev,uid ) ;
				free( dev ) ;		
			}else{
				printf( "ERROR: could not get volume info,is the volume opened?\n" ) ;
				st = 1 ;
			}
		}else{
			st = zuluCryptEXEVolumeInfo( strrchr( device,'/' ) + 1,device,uid ) ;
		}
	}else{
		p = String( UUID ) ;
		StringRemoveString( p,"\"" ) ;
		e = StringSubChar( p,4,'-' ) ;
		if( StringPrefixEqual( device,"/dev/loop" ) ){
			/*
			 * zuluCryptLoopDeviceAddress_1() is defined in ../zuluCrypt-cli/lib/create_loop_device.c
			 */
			dev = zuluCryptLoopDeviceAddress_1( device ) ;
			if( dev != NULL ){
				st = zuluCryptEXEVolumeInfo( e,dev,uid ) ;
				free( dev ) ;
			}else{
				printf( "ERROR: could not get volume info,is the volume opened?\n" ) ;
				st = 1 ;
			}
		}else{
			st = zuluCryptEXEVolumeInfo( e,device,uid ) ;
		}
		StringDelete( &p ) ;
	}
	
	return st ;
}

static int _zuluMountPrintDeviceProperties( const char * device,const char * UUID,uid_t uid )
{	
	string_t p ;
	string_t q ;
	string_t z = StringVoid ;
	char * dev = NULL ;
	const char * device_1 ;
	const char * e ;
	
	device_1 = device ;
	
	if( UUID == NULL ){
		if( StringPrefixEqual( device,"/dev/loop" ) ){
			/*
			* zuluCryptLoopDeviceAddress() is defined in ../zuluCrypt-cli/lib/create_loop_device.c
			*/
			device = dev = zuluCryptLoopDeviceAddress( device ) ;
			
			z = String( dev ) ;
			device_1 = StringReplaceString( z,"\\040"," " ) ;
			/*
			 * ZULUCRYPTlongMapperPath is set in ../zuluCrypt-cli/constants.h
			 * zuluCryptCreateMapperName() is defined at ../zuluCrypt-cli/lib/create_mapper_name.c
			 */
			q = zuluCryptCreateMapperName( device_1,strrchr( device_1,'/' ) + 1,uid,ZULUCRYPTlongMapperPath ) ;
		}else{
			/*
			 * ZULUCRYPTlongMapperPath is set in ../zuluCrypt-cli/constants.h
			 * zuluCryptCreateMapperName() is defined at ../zuluCrypt-cli/lib/create_mapper_name.c
			 */
			q = zuluCryptCreateMapperName( device,strrchr( device,'/' ) + 1,uid,ZULUCRYPTlongMapperPath ) ;
		}
	}else{
		p = String( UUID ) ;
		StringRemoveString( p,"\"" ) ;
		e = StringReplaceString( p,"UUID=","UUID-" ) ;
		q = zuluCryptCreateMapperName( device,e,uid,ZULUCRYPTlongMapperPath ) ;
		StringDelete( &p ) ;
	}
	
	e = StringContent( q ) ;
	
	/*
	 * zuluCryptGetMtabEntry() is defined in ../zuluCrypt-cli/lib/process_mountinfo.c
	 * zuluCryptPathIsValid() is defined in ../zuluCrypt-cli/lib/is_path_valid.c
	 */
	if( zuluCryptPathIsValid( e ) ){
		p = zuluCryptGetMtabEntry( e ) ;
	}else{
		p = zuluCryptGetMtabEntry( device ) ;
	}
	
	StringDelete( &q ) ;

	if( p != StringVoid ){
		/*
		* zuluMountPrintDeviceProperties() is defined in ./print_mounted_volumes.c
		*/
		zuluMountPrintDeviceProperties_1( p,uid ) ;
		StringDelete( &p ) ;
	}else{
		/*
		 * zuluMountPartitionProperties is defined in ./print_mounted_volumes.c
		 */
		zuluMountPartitionProperties( device_1,NULL,device_1,NULL ) ;
	}
	
	if( dev != NULL ){
		free( dev ) ;
	}
	
	StringDelete( &z ) ;
	
	return 0 ;
}

static int _zuluPartitionHasCryptoFs( const char * device )
{
	int st ;
	string_t fs = StringVoid ;
	/*
	* zuluCryptSecurityGainElevatedPrivileges() is defined in ../zuluCrypt-cli/bin/security.c
	*/
	zuluCryptSecurityGainElevatedPrivileges() ;
	/*
	 * zuluCryptGetFileSystemFromDevice() is defined in ../zuluCrypt-cli/lib/mount_volume.c
	 */
	fs = zuluCryptGetFileSystemFromDevice( device ) ;
	/*
	 * zuluCryptSecurityDropElevatedPrivileges() is defined in ../zuluCrypt-cli/bin/security.c
	 */
	zuluCryptSecurityDropElevatedPrivileges() ;
	if( fs == StringVoid ){
		/*
		 * no file system is found,assuming the volume is crypto_PLAIN volume
		 */	
		return 1 ;
	}else{
		/*
		 * st == 1 means the volume is cryto_LUKS
		 * st == 0 means the volume has a regular file system
		 */
		st = StringEqual( fs,"crypto_LUKS" ) ;
		StringDelete( &fs ) ;
		return st ;
	}
}

static int _zuluMountPrintVolumeDeviceName( const char * device )
{	
	ssize_t index ;
	string_t st = StringVoid ;
	char * c ;
	/*
	 * zuluCryptSecurityGainElevatedPrivileges() is defined in ../zuluCrypt-cli/bin/security.c
	 */
	zuluCryptSecurityGainElevatedPrivileges() ;
	
	st = String( device ) ;
	StringReplaceString( st,"/dev/","/dev/mapper/" ) ;
	index = StringLastIndexOfChar( st,'/' ) ;
	if( index != -1 ){
		device = StringSubChar( st,index,'-' ) ;
	}
	/*
	* zuluCryptVolumeDeviceName() is defined in ../lib/status.c
	*/
	c = zuluCryptVolumeDeviceName( device ) ;
	StringDelete( &st ) ;
	
	/*
	 * zuluCryptSecurityDropElevatedPrivileges() is defined in ../zuluCrypt-cli/bin/security.c
	 */
	zuluCryptSecurityDropElevatedPrivileges() ;
	if( c == NULL ){
		return 1 ;
	}else{
		printf( "%s\n",c ) ;
		free( c ) ;
		return 0 ;
	}
}

/*
 * This is an emergency function,to be used when a device was unplugged uncleanly causing the device
 * old path to be "locked" as pluggin in the device again will give it a different path 
 */
static int _checkUnmount( const char * device,uid_t uid )
{
	stringList_t stx ;
	stringList_t stl ;
	ssize_t index ;
	int r ;
	char * m_point = NULL ;
	string_t st ;
	const char * g ;
	
	if( StringPrefixEqual( device,"/dev/mapper/zuluCrypt-" ) ){
		/*
		 * encrypted volumes are handled someplace else
		 */
		return 0 ;
	}
	/*
	 * zuluCryptGetMoutedListFromMountInfo() is defined in ../lib/process_mountinfo.c 
	 */
	stx = zuluCryptGetMoutedListFromMountInfo() ;
	zuluCryptSecurityGainElevatedPrivileges() ;
	
	index = StringListHasStartSequence( stx,device ) ;
	
	if( index != -1 ){
		/*
		 * zuluCryptBindUnmountVolume() is defined in ../zuluCrypt-cli/bin/bind.c
		 */
		r = zuluCryptBindUnmountVolume( stx,device,uid ) ;
		if( r != 3 || r != 4 ){
			st = StringListStringAt( stx,index ) ;
			stl = StringListStringSplit( st,' ' ) ;
			st = StringListStringAt( stl,1 ) ;
			/*
			 * zuluCryptDecodeMtabEntry() is defined in ../zuluCrypt-cli/lib/mount_volume.c
			 */
			g  = zuluCryptDecodeMtabEntry( st ) ;
			/*
			 * zuluCryptSecurityMountPointPrefixMatch() is defined in ./security.c
			 */
			r = zuluCryptSecurityMountPointPrefixMatch( g,uid,NULL ) ;
			if( r == 1 ){
				/*
				* zuluCryptUnmountVolume() is defined in ../zuluCrypt-cli/lib/unmount_volume.c
				*/
				zuluCryptUnmountVolume( device,&m_point ) ;
				if( m_point != NULL ){
					rmdir( m_point ) ;
					free( m_point ) ;
				}
			}
			StringListDelete( &stl ) ;
		}
	}else{
		/*
		 * Either the volume is not mounted or is encrypted.
		 * If it is encrypted,then it is handled someplace else
		 */
		;
	}
	
	StringListDelete( &stx ) ;
	
	zuluCryptSecurityDropElevatedPrivileges() ;
	
	return 0 ;
}

static int _zuluMountExe( ARGS * args )
{
	const char * device = args->device ;
	const char * action = args->action ;
	const char * uuid   = args->uuid   ;
	size_t       uid    = args->uid    ;   
	
	if( StringsAreEqual( action,"-D" ) ){
		return _zuluMountPrintVolumeDeviceName( device ) ;
	}
	if( StringsAreEqual( action,"-L" ) ){
		return _zuluMountPrintDeviceProperties( device,uuid,uid ) ;
	}
	if( StringsAreEqual( action,"-s" ) ){
		return zuluMountVolumeStatus( device,uuid,uid ) ;
	}
	if( StringsAreEqual( action,"-m" ) ){
		if( _zuluPartitionHasCryptoFs( device ) ){
			/*
			 * zuluMountMount() is defined in crypto_mount.c
			 */
			return zuluMountCryptoMount( args ) ;
		}else{
			/*
			 * zuluMountMount() is defined in mount.c
			 */
			return zuluMountMount( args ) ;
		}
	}
	
	if( StringsAreEqual( action,"-u" ) ){
		if( _zuluPartitionHasCryptoFs( device ) ){
			/*
			 * zuluMountMount() is defined in crypto_umount.c
			 */
			return zuluMountCryptoUMount( args ) ;
		}else{
			/*
			 * zuluMountMount() is defined in umount.c
			 */
			return zuluMountUMount( args ) ;
		}
	}
	
	return _zuluExit_1( 200,StringListVoid,NULL,"ERROR: unrecognized argument encountered" ) ;	
}

static int _mount_help()
{
	const char * doc4 ;
	const char * doc3 ;
	const char * doc2 ;
	const char * doc1 = "\
options:\n\
-m -- mount a volume : arguments: -d partition_path -z mount_point -e mode(rw/ro)\n\
      -- additional arguments for crypto_LUKS,crypto_PLAIN,crypto_TCRYPT volumes, -p passphrase/-f keyfile\n\
-z -- mount point component to append to \"/run/media/private/$USER/\n\
-H -- open a truecrypt hidden volume\n" ;

	doc2 = "\
-u -- unmount a partition: arguments: -d partition_path\n\
-s -- print properties of an encrypted volume: arguments: -d partition_path\n\
-M -- this option will create a mount point in \"/run/media/private/$USER\" and a publicly accessible \"mirror\" in \"/run/media/public/\'\n";

      doc3 = "\
-l -- print expanded list of all partitions\n\
-L -- must be used with -d,print properties of a partition specified by d option\n\
-P -- print a list of all partitions\n\
-D -- get a device node address from its mapper path( mapper paths are usually located in /dev/mapper ). Required argument: -d\n";
	
      doc4= "\
examples:\n\
mount a volume  : zuluMount-cli -m -d /dev/sdc1\n\
unmount a volume: zuluMount-cli -u -d /dev/sdc1\n\
mount and encrypted volume with a key \"xyz\" : zuluMount-cli -m -d /dev/sdc2 -p xyz\n" ;
      
      printf( "%s%s%s%s",doc1,doc2,doc3,doc4 ) ;
	
	return 201 ;
}

static void ExitOnMemoryExaustion( void )
{
	printf( "unexpected exiting because you have run out of memory\n" ) ;
	exit( 1 ) ;
}

static int _zuluMountDoAction( ARGS * args )
{	
	int fd = -1 ;
	int fd1 = -1 ;
	int status ;
	char * dev = NULL ;
	const char * msg = "ERROR: a non supported device encountered,device is missing or permission denied\n\
Possible reasons for getting the error are:\n1.Device path is invalid.\n2.The device has LVM or MDRAID signature\n" ;
	/*
	 * zuluCryptGetDeviceFileProperties is defined in ../zuluCrypt-lib/file_path_security.c
	 */
	switch( zuluCryptGetDeviceFileProperties( args->device,&fd,&fd1,&dev,args->uid ) ){
		case 0 : args->device = dev ;
			 status = _zuluMountExe( args ) ; 
			 if( dev != NULL ){
				 free( dev ) ;
			 }
			 if( fd1 != -1 ){
				 close( fd ) ;
			 }
			 if( fd != -1 ){
				 close( fd ) ;
			 }
			 return status ;
		case 1 : printf( "ERROR: devices in /dev/shm path is not suppored\n" ) ;					return 220 ;
		case 2 : printf( "ERROR: given path is a directory\n" ) ;  					 		return 221 ;
		case 3 : printf( "ERROR: a file can have only one hard link\n" ) ;				 		return 222 ;
		case 4 : printf( "ERROR: insufficient privilges to access the device\n" ) ; 					return 223 ;
		default: printf( msg ) ; 											return 224 ;
	}
}

static void _privilegeEvelationError( const char * msg )
{
	puts( msg ) ;
	exit( 255 ) ;
}

static void _forceTerminateOnSeriousError( int sig )
{
	if( sig ){;}
	puts( "SIGSEGV caught,exiting" ) ;
	exit( 255 ) ;
}

int main( int argc,char * argv[] )
{	
	char * action ;
	char * device ;
	string_t * k ;
	stringList_t stl ;
	stringList_t stx ;
	int status ;
	
	uid_t uid = getuid() ;
	gid_t gid = getgid() ;
	
	/*
	 * ARGS structure is declared in ./includes.h
	 */
	ARGS args ;
	
	struct sigaction sa ;
	memset( &sa,'\0',sizeof( struct sigaction ) ) ;
	sa.sa_handler = _forceTerminateOnSeriousError ;
	sigaction( SIGSEGV,&sa,NULL ) ;
	
	if( argc < 2 ){
		return _mount_help() ;
	}
	if( argc == 2 ){
		action = argv[ 1 ] ;
		if ( StringsAreEqual( action,"-h" ) || 
			StringsAreEqual( action,"--help" ) || 
			StringsAreEqual( action,"-help" ) ){
			return _mount_help() ;
		}
		if ( StringsAreEqual( action,"-v" ) || 
			StringsAreEqual( action,"-version" ) || 
			StringsAreEqual( action,"--version" ) ){
			printf( "%s\n",zuluCryptVersion() );
			return 0 ;
		}
	}
	
	memset( &args,'\0',sizeof( args ) ) ;
	
	args.uid = uid ;
	
	/*
	 * setgroups() requires seteuid(0) ;
	 */
	seteuid( 0 ) ;
	if( setgroups( 1,&gid ) != 0 ){
		_privilegeEvelationError( "ERROR: setgroups() failed" ) ;
	}
	if( setegid( uid ) != 0 ){
		_privilegeEvelationError( "ERROR: setegid() failed" ) ;
	}
	setuid( 0 ) ;
	seteuid( uid ) ;
	
	/*
	 * zuluCryptClearDeadMappers() is defined in ../zuluCrypt-cli/bin/clear_dead_mapper.c
	 */
	zuluCryptClearDeadMappers( uid ) ;
	
	/*
	 * zuluCryptSecuritySetPrivilegeElevationErrorFunction() is defined in ../zuluCrypt-cli/bin/security.c
	 * _privilegeEvelationError() function will be called when functions that elevate or drop privilges fail
	 */
	zuluCryptSecuritySetPrivilegeElevationErrorFunction( _privilegeEvelationError ) ;
	
	/*
	 * zuluCryptGetUserUIDForPrivilegeManagement() is defined in ../zuluCrypt-bin/security.c
	 */
	zuluCryptGetUserUIDForPrivilegeManagement( uid ) ;
	/*
	 * zuluCryptSecurityDropElevatedPrivileges() is defined in ../zuluCrypt-cli/bin/security.c
	 */
	zuluCryptSecurityDropElevatedPrivileges() ;
		
	StringExitOnMemoryExaustion( ExitOnMemoryExaustion ) ;
	StringListExitOnMemoryExaustion( ExitOnMemoryExaustion ) ;
	SocketExitOnMemoryExaustion( ExitOnMemoryExaustion ) ;
	ProcessExitOnMemoryExaustion( ExitOnMemoryExaustion ) ;
	
	if( _mount_get_opts( argc,argv,&args ) != 0 ){
		return _mount_help() ;
	}
	
	stl = StringListInit() ; 
	/*
	 * zuluCryptSecuritySanitizeTheEnvironment() is defined in ../zuluCrypt-cli/bin/security.c
	 */
	zuluCryptSecuritySanitizeTheEnvironment( global_variable_user_uid,&stx ) ;
	
	if( args.key != NULL ){
		k = StringListAssign( stl ) ;
		*k = String( args.key ) ;
		strncpy( ( char * ) args.key,"x",StringLength( *k ) ) ;
		args.key = StringContent( *k ) ;
	}
	
	if( args.action == NULL ){
		return _zuluExit_2( 212,stl,stx,"ERROR: action not specified" ) ;
	}
	if( StringsAreEqual( args.action,"-S" ) ){
		/*
		 * zuluCryptPrintPartitions() is defined in ../zuluCrypt-cli/bin/partitions.c
		 * it printf() devices with entries in "/etc/fstab","/etc/crypttab", and "/etc/zuluCrypttab"
		 * ZULUCRYPTsystemPartitions is set in ../zuluCrypt-cli/constants.h
		 */
		return _zuluExit_2( zuluCryptPrintPartitions( ZULUCRYPTsystemPartitions,0 ),stl,stx,NULL ) ;
	}
		
	if( StringsAreEqual( args.action,"-c" ) ){
		return _checkUnmount( args.device,uid ) ;
	}
	if( StringsAreEqual( args.action,"-l" ) ){
		return _zuluExit_2( _zuluMountMountedList( uid ),stl,stx,NULL ) ;
	}
	if( StringsAreEqual( args.action,"-P" ) ){
		return _zuluExit_2( _zuluMountDeviceList( uid ),stl,stx,NULL ) ;
	}
	if( StringsAreEqual( args.action,"-h" ) ){
		return _zuluExit_2( _mount_help(),stl,stx,NULL ) ;
	}
	if( args.device == NULL ){
		return _zuluExit_2( 213,stl,stx,"ERROR: device argument missing" ) ;
	}
	if( args.m_opts == NULL ){
		args.m_opts = "rw" ;
	}
	
	args.env = stx ;
	
	/*
	 * zuluCryptSecurityEvaluateDeviceTags() is defined in ../zuluCrypt-cli/bin/security.c
	 */
	if( StringPrefixEqual( args.device,"UUID=" ) ){
		device = zuluCryptSecurityEvaluateDeviceTags( "UUID",args.device + 5 ) ;
		if( device != NULL ){
			args.uuid = args.device ;
			args.device = device ;
			status = _zuluMountDoAction( &args ) ;
			free( device ) ;
		}else{
			printf( "could not resolve UUID\n" ) ;
			status = 214 ;
		}
	}else if( StringPrefixEqual( args.device,"LABEL=" ) ){
		device = zuluCryptSecurityEvaluateDeviceTags( "LABEL",args.device + 6 ) ;
		if( device != NULL ){
			args.device = device ;
			status = _zuluMountDoAction( &args ) ;
			free( device ) ;
		}else{
			printf( "could not resolve LABEL\n" ) ;
			status = 215 ;
		}
	}else{
		status = _zuluMountDoAction( &args ) ;
	}
	
	return _zuluExit_2( status,stl,stx,NULL ) ;
}
