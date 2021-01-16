#pragma once

//#include "Utils.hpp"
#include <QThread>
#include <qDebug>
#include "Utils.h"
#include <QTime>
#include <QFile>
#include <QList>
#include <QTextCodec>

class AnalyseThread : public QThread
{
	Q_OBJECT

public:
	AnalyseThread(QObject* parent = nullptr) {

	}

	//�����ļ���ַ
	void setFilePath(QString path) {
		mPath = path;
	}

	//�����ж�����
	void setIf(QString code, QString If_And, QString If_Or, QString If_Exclude) {
		AnalyseThread::If_And = If_And;
		AnalyseThread::If_Or = If_Or;
		AnalyseThread::If_Exclude = If_Exclude;
		AnalyseThread::Code = code;
	}

	//����ֹͣ�߳�
	void setStop() {
		isRuning = false;
	}

signals:
	void on_analyse_runing(const int& count, const long& elapsed); //������
	void on_analyse_end(const int& count, const long& elapsed);  //���߳�ִ�н���ʱ
	void on_analyse_log(const QList<QString>& log); //��ƥ�䵽һ����־��ʱ��

protected:
	//QThread���麯��
	//�̴߳�����
	//����ֱ�ӵ��ã�ͨ��start()��ӵ���
	void run() {
		//��������
		int count_line = 0;
		//��ʼʱ��
		QTime startTime = QTime::currentTime();


		QFile file(mPath);
		if (file.open(QIODevice::ReadOnly)) {

			//��ȡ����
			QList<QString> log_array;

			//���ڴ���ļ��ж�ȡ������
			QByteArray array;

			//�������İ�
			QString bright = If_And + ";" + If_Or;

			QTextCodec* mCodec = QTextCodec::codecForName(Code.toStdString().c_str());
			QTime time_segment = QTime::currentTime();

			while (!(array = file.readLine()).isEmpty() && isRuning) {


				QString data_trimmed = mCodec->toUnicode(array);
				//QString data_trimmed(array);

				if (!If_And.isEmpty() && !isContainsAnd(data_trimmed, If_And)) continue;
				if (!If_Or.isEmpty() && !isContainsOr(data_trimmed, If_Or)) continue;
				if (!If_Exclude.isEmpty() && isContainsOr(data_trimmed, If_Exclude)) continue;

				log_array.append(getBrightString(data_trimmed, bright));

				count_line++;

				int elapsed = time_segment.msecsTo(QTime::currentTime()); //��ʱ ms
				if (elapsed >= 13) {
					on_analyse_runing(count_line, startTime.msecsTo(QTime::currentTime()));
					time_segment = QTime::currentTime();
					msleep(1);
				}
			}
			file.close();
			if (isRuning && !log_array.isEmpty()) {
				//on_analyse_log(log_array.join("<br/>"));
				on_analyse_log(log_array);
				on_analyse_end(count_line, startTime.msecsTo(QTime::currentTime()));
			}
		}

	}

private:
	bool isRuning = true;
	QString mPath;
	QString If_And;
	QString If_Or;
	QString If_Exclude;
	QString Code;


};

