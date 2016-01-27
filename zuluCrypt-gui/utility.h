/*
 *
 *  Copyright ( c ) 2011-2015
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  ( at your option ) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MISCFUNCTIONS_H
#define MISCFUNCTIONS_H

#include <QString>
#include <QStringList>
#include <QEvent>
#include <QProcess>
#include <QThreadPool>
#include <QRunnable>
#include <QMetaObject>
#include <QDebug>
#include <QWidget>
#include <QDialog>
#include <QEventLoop>
#include <QTimer>
#include <QMenu>
#include <QVector>
#include <QSystemTrayIcon>
#include <QAction>
#include <QIcon>

#include <functional>
#include <memory>
#include <array>
#include <utility>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <blkid/blkid.h>

#include "task.h"
#include "lxqt_wallet/frontend/lxqt_wallet.h"

#include <QObject>
#include <QLabel>

#include <poll.h>
#include <fcntl.h>

class QByteArray ;
class QEvent ;

namespace utility
{
	template< typename T >
	class qObject_unique_ptr
	{
	public:
		explicit qObject_unique_ptr( T * t ) : m_qObject( t )
		{
		}

		qObject_unique_ptr( const qObject_unique_ptr& ) = delete ;
		qObject_unique_ptr& operator =( const qObject_unique_ptr& ) = delete ;

		qObject_unique_ptr( qObject_unique_ptr&& other )
		{
			this->deleteHandle() ;
			m_qObject = other.m_qObject ;
			other.m_qObject = nullptr ;
		}

		T * operator->()
		{
			return m_qObject ;
		}

		~qObject_unique_ptr()
		{
			this->deleteHandle() ;
		}
	private:
		void deleteHandle()
		{
			if( m_qObject ){

				m_qObject->deleteLater() ;
			}
		}

		T * m_qObject = nullptr ;
	};
}

namespace utility
{
	void setUID( int ) ;

	int getUID() ;
	int getUserID() ;

	QString getStringUserID() ;
	QString appendUserUID( const QString& ) ;
	QString homePath() ;

	template< typename T >
	void changeFileOwner( const T& f )
	{
		int uid = utility::getUID() ;
		int fd = f.handle() ;

		if( uid != -1 && fd != -1 ){

			if( fchown( fd,uid,uid ) ){;}
		}
	}

	static inline void changeFileOwner( const char * path )
	{
		int uid = utility::getUID() ;

		if( uid != -1 ){

			if( chown( path,uid,uid ) ){;}
		}
	}

	template< typename T >
	void changeFilePermissions( const T& f,int mode = 0777 )
	{
		if( fchmod( f.handle(),mode ) ){;}
	}
}

namespace utility
{
	class Array
	{
	public:
		explicit Array( const QString&,char splitter = '\n' ) ;
		explicit Array( const QStringList& ) ;
		size_t size() ;
		char * const * value() ;
	private :
		void setUp() ;
		QVector< const char * > m_vector ;
		QList< QByteArray > m_list ;
	};

	struct wallet
	{
		bool opened ;
		bool notConfigured ;
		QString key ;
		QString password ;
	};

	int startApplication( const char * appName,std::function<int()> ) ;

	wallet getKeyFromWallet( LxQt::Wallet::walletBackEnd,const QString& keyID,const QString& pwd = QString() ) ;

	QString cmdArgumentValue( const QStringList&,const QString& arg,const QString& defaulT = QString() ) ;

	QIcon getIcon( const QString& ) ;

	bool userBelongsToGroup( const char * groupname ) ;
	bool runningInMixedMode( void ) ;
	bool notRunningInMixedMode( void ) ;
	void addToFavorite( const QString& dev,const QString& m_point ) ;
	QStringList readFavorites( void ) ;
	void readFavorites( QMenu *,bool = false ) ;
	void removeFavoriteEntry( const QString& ) ;
	bool pathExists( const QString& ) ;
	bool canCreateFile( const QString& ) ;
	QString resolvePath( const QString& ) ;
	QString hashPath( const QByteArray& ) ;
	QString cryptMapperPath( void ) ;
	QString mapperPath( const QString&,const QString& component = QString() ) ;
	QString getVolumeID( const QString&,bool = false ) ;
	bool userIsRoot( void ) ;
	bool mapperPathExists( const QString& path ) ;
	QString mountPath( const QString& ) ;
	QString homeMountPath( const QString& ) ;
	QString mountPathPostFix( const QString& ) ;
	QString userName( void ) ;
	QString shareMountPointToolTip( void ) ;
	QString shareMountPointToolTip( const QString& ) ;
	QString sharedMountPointPath( const QString& ) ;
	bool pathPointsToAFile( const QString& ) ;
	bool pathPointsToAFolder( const QString& ) ;
	QString localizationLanguage( const QString& ) ;
	QString localizationLanguagePath( const QString& ) ;
	void setLocalizationLanguage( const QString&,const QString& ) ;
	QString walletName( void ) ;
	QString applicationName( void ) ;
	bool pathIsReadable( const QString& ) ;
	bool pathIsWritable( const QString& ) ;
	bool setOpenVolumeReadOnly( QWidget * parent,bool check,const QString& app ) ;
	bool getOpenVolumeReadOnlyOption( const QString& app ) ;
	QString keyPath( void ) ;
	void keySend( const QString& keyPath,const QString& key ) ;
	bool eventFilter( QObject * gui,QObject * watched,QEvent * event,std::function< void() > ) ;
	QStringList split( const QString&,char token = '\n' ) ;
	QStringList split( const QByteArray&,char token = '\n' ) ;
	QStringList directoryList( const QString& ) ;
	bool userHasGoodVersionOfWhirlpool( void ) ;
	void licenseInfo( QWidget * ) ;
	void showTrayIcon( QAction *,QSystemTrayIcon *,bool = true ) ;
	void trayProperty( QSystemTrayIcon *,bool = true ) ;

	bool reUseMountPointPath( void ) ;

	void setLocalizationLanguage( bool translate,QWidget * obj,QAction * ac,const char * ) ;
	void languageMenu( QWidget *,QMenu *,QAction *,const char * ) ;

	bool ProcessExecute( const QString& m_point,const QString& exe,const QString& env,int uid ) ;

	using array_t = std::array< int,10 > ;

	utility::array_t getWindowDimensions( const QString& application ) ;
	void setWindowDimensions( const QString& application,const std::initializer_list<int>& ) ;

	void createPlugInMenu( QMenu *,const QString&,const QString&,const QString&,bool ) ;

	int pluginKey( QDialog *,QString *,const QString& ) ;

	QFont getFont( QWidget * ) ;
	void saveFont( const QFont& ) ;

	::Task::future< int >& clearVolume( const QString& volumePath,bool * exit,std::function< void( int ) > ) ;
	::Task::future< int >& exec( const QString& ) ;
	::Task::future< QStringList >& luksEmptySlots( const QString& volumePath ) ;
	::Task::future< QString >& getUUIDFromPath( const QString& ) ;
	::Task::future<QString>& getKeyFromWallet( LxQt::Wallet::Wallet * wallet,const QString& volumeID ) ;
	::Task::future< bool >& openPath( const QString& path,const QString& opener,const QString& env = QString() ) ;

	void openPath( const QString& path,const QString& opener,const QString& env,QWidget *,const QString&,const QString& ) ;
}

namespace utility
{
	class fileHandle
	{
	public:
		fileHandle()
		{
		}
		fileHandle( int r ) : m_fd( r )
		{
		}
		fileHandle( int r,std::function< void( int ) > cmd ) :
			m_fd( r ),m_releaseResource( std::move( cmd ) )
		{
		}
		bool open( const char * filePath,bool ro = true )
		{
			if( ro ){

				m_fd = ::open( filePath,O_RDONLY ) ;
			}else{
				m_fd = ::open( filePath,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH ) ;
			}

			m_path = filePath ;

			return m_fd != -1 ;
		}
		bool open( const QString& filePath,bool ro = true )
		{
			return this->open( filePath.toLatin1().constData(),ro ) ;
		}
		bool isFile()
		{
			struct stat st ;
			fstat( m_fd,&st ) ;
			return S_ISREG( st.st_mode ) != 0 ;
		}
		bool isFolder()
		{
			struct stat st ;
			fstat( m_fd,&st ) ;
			return S_ISDIR( st.st_mode ) != 0 ;
		}
		quint64 size()
		{
			return static_cast< quint64 >( blkid_get_dev_size( m_fd ) ) ;
		}
		void unlink()
		{
			m_unlink = true ;
		}
		int handle() const
		{
			return m_fd ;
		}
		const char * path()
		{
			return m_path.constData() ;
		}
		bool opened() const
		{
			return m_fd != -1 ;
		}
		char getChar() const
		{
			char z ;

			while( true ){

				while( ::read( m_fd,&z,1 ) != 1 ){;}

				if( z > ' ' && z < '~' ){

					/*
					 * we are creating a keyfile that is made up
					 * of only printable characters
					 */
					break ;
				}
			}

			return z ;
		}
		void writeChar( char r ) const
		{
			if( ::write( m_fd,&r,1 ) ){;}
		}
		~fileHandle()
		{
			m_releaseResource( m_fd ) ;

			if( m_unlink ){

				::unlink( m_path.constData() ) ;
			}
		}
	private:
		bool m_unlink = false ;

		int m_fd = -1 ;

		QByteArray m_path ;

		std::function< void( int ) > m_releaseResource = []( int fd ){

			if( fd != -1 ){

				::close( fd ) ;
			}			
		} ;
	} ;
}

namespace utility
{
	class monitor_mountinfo
	{
	public:
		monitor_mountinfo()
		{
			m_handle.open( "/proc/self/mountinfo" ) ;
			m_monitor.fd     = m_handle.handle() ;
			m_monitor.events = POLLPRI ;
		}
		operator bool()
		{
			return m_handle.opened() ;
		}
		bool gotEvent() const
		{
			poll( &m_monitor,1,-1 ) ;
			return true ;
		}
	private:
		utility::fileHandle m_handle ;
		mutable struct pollfd m_monitor ;
	};
}

namespace utility
{
	class Task
	{
	public :
		static ::Task::future< utility::Task >& run( const QString& exe )
		{
			return ::Task::run< utility::Task >( [ exe ](){ return utility::Task( exe ) ; } ) ;
		}
		static void wait( int s )
		{
			sleep( s ) ;
		}
		static void waitForOneSecond( void )
		{
			sleep( 1 ) ;
		}
		static void waitForTwoSeconds( void )
		{
			sleep( 2 ) ;
		}
		static void suspendForOneSecond( void )
		{
			utility::Task::suspend( 1 ) ;
		}
		static void suspend( int s )
		{
			QTimer t ;

			QEventLoop l ;

			QObject::connect( &t,SIGNAL( timeout() ),&l,SLOT( quit() ) ) ;

			t.start( 1000 * s ) ;

			l.exec() ;
		}
		Task()
		{
		}
		Task( const QString& exe,int waitTime = -1 )
		{
			QProcess p ;
			p.start( exe ) ;
			m_finished   = p.waitForFinished( waitTime ) ;
			m_exitCode   = p.exitCode() ;
			m_exitStatus = p.exitStatus() ;
			m_data       = p.readAll() ;
		}
		QStringList splitOutput( char token ) const
		{
			return utility::split( m_data,token ) ;
		}
		void output( const QByteArray& r )
		{
			m_data = r ;
		}
		const QByteArray& output() const
		{
			return m_data ;
		}
		int exitCode() const
		{
			return m_exitCode ;
		}
		int exitStatus() const
		{
			return m_exitStatus ;
		}
		bool success() const
		{
			return m_exitCode == 0 && m_exitStatus == QProcess::NormalExit && m_finished == true ;
		}
		bool failed() const
		{
			return !this->success() ;
		}
		bool finished() const
		{
			return m_finished ;
		}
		bool ok() const
		{
			return this->splitOutput( '\n' ).size() > 12 ;
		}
	private:
		QByteArray m_data ;
		int m_exitCode ;
		int m_exitStatus ;
		bool m_finished ;
	};
}

namespace utility
{
	class veraCryptWarning : public QObject
	{
		Q_OBJECT
	public:
		veraCryptWarning()
		{
			connect( &m_timer,SIGNAL( timeout() ),this,SLOT( update() ) ) ;
		}
		void setWarningLabel( QLabel * l )
		{
			m_label = l ;
			m_label->setVisible( false ) ;
		}
		void show( QString w )
		{
			m_warning = std::move( w ) ;
			this->show( true ) ;
		}
		void stopTimer()
		{
			m_timer.stop() ;
		}
		void show( bool show )
		{
			if( show ){
				m_label->setVisible( true ) ;
				m_timer.start( 1000 * 1 ) ;
				this->update() ;
			}
		}
		void hide()
		{
			m_timer.stop() ;
			m_time = 0 ;
			m_label->setVisible( false ) ;
			m_label->setText( m_warning + tr( "Elapsed time: 0 seconds" ) ) ;
		}
	private slots:
		void update()
		{
			QString e ;
			if( m_time >= 60 ){

				e = tr( "Elapsed time: %0 minutes" ).arg( QString::number( m_time / 60,'f',2 ) ) ;
			}else{
				e = tr( "Elapsed time: %0 seconds" ).arg( QString::number( m_time ) ) ;
			}

			m_time++ ;

			m_label->setText( m_warning + e ) ;
		}
	private:
		QLabel * m_label ;
		QTimer m_timer ;
		float m_time = 0 ;
		QString m_warning = tr( "Please be patient as unlocking a VeraCrypt volume may take a very long time.\n\n" ) ;
	};
}
#endif // MISCFUNCTIONS_H
