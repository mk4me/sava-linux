#ifndef LOG_H
#define LOG_H

#include <QtWidgets/QWidget>
#include "ui_Logger.h"

#include <memory>
#include <QtCore/QProcess>

#include <QtCore/QFile>

class Logger : public QWidget
{
	Q_OBJECT

public:
	//! Constructor.
	/*!
	\param parent of this window.
	*/
	Logger(QWidget *parent);
	//! Destructor.
	~Logger();

	//! Sets path where logs should be saved.
	/*!
	\param _filePath where logs should be saved.
	*/
	void setFilePath(const QString& _filePath);

	//! Fills up listWidget.
	/*!
	\param _log reference message that comes from process.
	\param _isError is this error message?
	*/
	void log(const QString& _log, enum Qt::GlobalColor _color = Qt::black);

private:
	//! Returns time stamp in format [dd/MM/yyyy h:mm:ss zzz]
	QString getTimeStamp();
	
	//! Object that holds ui configuration.
	Ui::Logger ui;
	//! Shared pointer to QProcess.
	std::shared_ptr<QProcess> proc_;

	//! Is there a fixed number for debug history.
	bool isDebugHistoryFixed_;
	//! How many lines will be kept in log window.
	int maxLogLines_;

	//! When someone moves slider.
	bool sliderMoved_ = false;
	//! Maximum range of slider.
	int maxSliderRange_ = 0;

	//! Should log to file?
	bool logToFile_ = false;
	//! QFile shared pointer.
	std::shared_ptr<QFile> file_;
	//! QTextStream shared pointer.
	std::shared_ptr<QTextStream> stream_;

private slots:
	//! Triggers when sliders move.
	/*!
	\param _val action that happened.
	*/
	void actionTriggered(int _val);
	//! Triggers when sliders move.
	/*!
	\param _val position of slider.
	*/
	void valueChanged(int _val);
	//! Triggers when new item goes to list.
	/*!
	\param _min new min range for slider.
	\param _max new max range for slider.
	*/
	void rangeChanged(int _min, int _max);
};

#endif // LOG_H
