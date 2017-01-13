#include "Logger.h"
#include "config/Diagnostic.h"
#include <QtCore/QDateTime>

#include <QtCore/QDebug>

Logger::Logger(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);

	config::Diagnostic::getInstance().load();
	maxLogLines_ = config::Diagnostic::getInstance().getDebugHistory();
	//jezeli mniejsze od 1 to historia jest nieskonczona
	isDebugHistoryFixed_ = maxLogLines_ > 0;

	//przeiwjanie tekstu tylko gdy slider jest na samym dole
	QObject::connect((QObject*)ui.listWidget->verticalScrollBar(), SIGNAL(actionTriggered(int)),
		this, SLOT(actionTriggered(int)));
	QObject::connect((QObject*)ui.listWidget->verticalScrollBar(), SIGNAL(valueChanged(int)),
		this, SLOT(valueChanged(int)));
	QObject::connect((QObject*)ui.listWidget->verticalScrollBar(), SIGNAL(rangeChanged(int, int)),
		this, SLOT(rangeChanged(int, int)));	
}

Logger::~Logger()
{
	ui.listWidget->disconnect();
}

void Logger::setFilePath(const QString& _filePath)
{
	//logowanie do pliku
	logToFile_ = config::Diagnostic::getInstance().getLogToFile();
	if (logToFile_)
	{
		file_ = std::make_shared<QFile>(_filePath);
		file_->open(QFile::WriteOnly | QFile::Truncate);
		file_->close();

		stream_ = std::make_shared<QTextStream>(file_.get());
	}
}

void Logger::log(const QString& _log, enum Qt::GlobalColor _color)
{
	if (isDebugHistoryFixed_ &&  ui.listWidget->count() > maxLogLines_)
	{
		auto item = ui.listWidget->takeItem(0);
		delete item;
	}

	QListWidgetItem* item = new QListWidgetItem(getTimeStamp() + _log.trimmed());
	item->setForeground(_color);
	ui.listWidget->addItem(item);

	if (!sliderMoved_)
		ui.listWidget->scrollToBottom();

	if (logToFile_)
	{
		file_->open(QFile::WriteOnly | QFile::Append);		
		(*stream_.get()) << getTimeStamp() + _log.trimmed() << endl;
		file_->close();
	}
}

QString Logger::getTimeStamp()
{
	QDateTime dateTime = QDateTime::currentDateTime();
	return dateTime.toString("[dd/MM/yyyy h:mm:ss zzz]: ");
}

void Logger::actionTriggered(int _val)
{
	sliderMoved_ = true;
}

void Logger::valueChanged(int _val)
{
	if (sliderMoved_ && _val == maxSliderRange_)
		sliderMoved_ = false;
}

void Logger::rangeChanged(int _min, int _max)
{
	maxSliderRange_ = _max;
}