#include "managepartitionthread.h"

managepartitionthread::managepartitionthread()
{
}

void managepartitionthread::setMode( QString mode )
{
	m_mode = mode ;
}

void managepartitionthread::setDevice( QString path )
{
	m_device = path ;
}

void managepartitionthread::setType( QString type )
{
	m_type = type ;
}

void managepartitionthread::setKeySource( QString key )
{
	m_keySource = key ;
}

void managepartitionthread::run()
{
	if( m_action == QString( "update") ){

		this->partitionList();

	}else if( m_action == QString( "partitionList" ) ){

		this->partitionList();

	}else if( m_action == QString( "mount" ) ){

		this->mount();

	}else if( m_action == QString( "umount" ) ){

		this->umount( m_type );

	}else if( m_action == QString( "cryptoOpen" ) ){

		this->cryptoOpen();
	}
}

void managepartitionthread::partitionList()
{
	QProcess p ;
	QStringList list ;

	p.start( QString( "%1 -l" ).arg( zuluMount ) ) ;
	p.waitForFinished() ;

	list = QString( p.readAll() ).split( '\n' ) ;
	p.close();
	emit signalMountedList( list ) ;
}

void managepartitionthread::cryptoOpen()
{
	QProcess p ;
	QString exe ;

	m_point = QDir::homePath() + QString( "/" ) + m_device.split( "/" ).last() ;

	exe = QString( "%1 -M -d \"%2\" -z \"%3\" -e %4 %5" ).arg( zuluMount ).arg( m_device ).arg( m_point ).arg( m_mode ).arg( m_keySource ) ;

	p.start( exe );
	p.waitForFinished() ;

	QString output = QString( p.readAll() ) ;
	int index = output.indexOf( QChar( ':') ) ;
	if( index != -1 )
	output = output.mid( index + 1 ) ;
	emit signalMountComplete( p.exitCode(),output ) ;
	p.close();
}

void managepartitionthread::mount()
{
	QProcess p ;
	QString exe ;

	QString mount_p = QDir::homePath() + QString( "/" ) + m_device.split( "/" ).last() ;

	exe = QString( "%1 -m -d \"%2\" -e %3 -z \"%4\"" ).arg( zuluMount ).arg( m_device ).arg( m_mode ).arg( mount_p ) ;

	p.start( exe );
	p.waitForFinished() ;

	QString output = QString( p.readAll() ) ;
	int index = output.indexOf( QChar( ':') ) ;
	output = output.mid( index + 1 ) ;
	emit signalMountComplete( p.exitCode(),output ) ;
	p.close();
}

void managepartitionthread::umount( QString type )
{
	QProcess p ;
	QString exe ;

	m_point = QDir::homePath() + QString( "/" ) + m_device.split( "/" ).last() ;

	if( type == QString( "crypto_LUKS" ) )
		exe = QString( "%1 -U -d \"%2\"" ).arg( zuluMount ).arg( m_device ) ;
	else
		exe = QString( "%1 -u -d \"%2\"" ).arg( zuluMount ).arg( m_device ) ;

	p.start( exe );
	p.waitForFinished() ;

	QString output = QString( p.readAll() ) ;
	int index = output.indexOf( QChar( ':') ) ;
	output = output.mid( index + 1 ) ;
	emit signalUnmountComplete( p.exitCode(),output ) ;
	p.close();
}

void managepartitionthread::startAction( QString action )
{
	m_action = action ;
	QThreadPool::globalInstance()->start( this ) ;
}