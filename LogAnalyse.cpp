#include "LogAnalyse.h"
 

//������
Filter::Filter(QString title, QString t_and, QString t_or, QString t_exclude) {
	text_title = title;
	text_and = t_and;
	text_or = t_or;
	text_exclude = t_exclude;
}

QString Filter::toString()
{
	//return "";
	return QObject::tr("%1 -> %2,%3,%4").arg(text_title, text_and, text_or, text_exclude);
}

bool Filter::operator==(const Filter& other) const
{
	return (text_title == other.text_title) && (text_and == other.text_and) && (text_or == other.text_or) && (text_exclude == other.text_exclude);
}

//UI
LogAnalyse::LogAnalyse(QWidget* parent)	: QMainWindow(parent)
{
	ui.setupUi(this);
	//ui.label->setTextInteractionFlags(Qt::TextSelectableByMouse); //��ѡ��

	ui.Edit_IfAnd->setPlaceholderText(From8Bit("һ����־�б��������ȫ�����ݣ���������ԡ�;���ŷָ�"));
	ui.Edit_IfOr->setPlaceholderText(From8Bit("һ����־��ֻ��Ҫ��������֮һ����������ԡ�;���ŷָ�"));
	ui.Edit_IfExclude->setPlaceholderText(From8Bit("һ����־���ܰ��������ݣ���������ԡ�;���ŷָ�"));

	ui.Edit_Create_IfAnd->setPlaceholderText(From8Bit("���������� And"));
	ui.Edit_Create_IfOr->setPlaceholderText(From8Bit("���������� Or"));
	ui.Edit_Create_IfExclude->setPlaceholderText(From8Bit("���������� Exclude"));
	ui.Edit_Create_Title->setPlaceholderText(From8Bit("���������⣨�������룩"));

	clear_AnalyseResult();

	//ui.Edit_InputFilePath->setText(From8Bit("C:/Users/A��Cap/Desktop/debug.log"));


	mFilterFilePath = qApp->applicationDirPath() + QDir::separator() + "filter.json";


	//��ʾ�������б�
	readFilterFile();
	int size = mFilterArray.size();
	for (int i = 0; i != size; i++) {
		Filter filter = mFilterArray.at(i);
		insertFilterToListWidget(i, filter);
	}
}

//���
void LogAnalyse::on_BT_Clear_clicked() {
	ui.Edit_IfAnd->setText("");
	ui.Edit_IfOr->setText("");
	ui.Edit_IfExclude->setText("");
}
//ѡ���ļ�
void LogAnalyse::on_BT_CheckFile_clicked() {

	QString fileName = QFileDialog::getOpenFileName(this, From8Bit("��ѡ����־�ļ�"), ".", tr("All files(*.*)"));
	if (!fileName.isEmpty()) {
		ui.Edit_InputFilePath->setText(fileName);
	}
}
//�������
void LogAnalyse::on_BT_Create_clicked()
{
	QString text_title = ui.Edit_Create_Title->text();
	QString text_if_and = ui.Edit_Create_IfAnd->text();
	QString text_if_or = ui.Edit_Create_IfOr->text();
	QString text_if_exclude = ui.Edit_Create_IfExclude->text();

	if (text_title.isEmpty()) {
		QMessageBox::information(NULL, From8Bit("ע��"), From8Bit("��������⣬�Ա����ܿ�ݵ��ҵ�������"), QMessageBox::Yes, QMessageBox::Yes);
		return;
	}

	if (text_if_and.isEmpty() && text_if_or.isEmpty() && text_if_exclude.isEmpty()) {
		QMessageBox::information(NULL, From8Bit("ע��"), From8Bit("��Ч�Ĺ�����,û�й�������!"), QMessageBox::Yes, QMessageBox::Yes);
		return;
	}

	//��ӹ�����
	Filter filter = Filter(text_title, text_if_and, text_if_or, text_if_exclude);
	mFilterArray.prepend(filter);
	writeFilterFile();
	insertFilterToListWidget(0, filter);

	//����
	ui.Edit_Create_Title->clear();
	ui.Edit_Create_IfAnd->clear();
	ui.Edit_Create_IfExclude->clear();
	ui.Edit_Create_IfOr->clear();
}

//��д�ļ���Ϣ
void LogAnalyse::writeFilterFile()
{
	QFile file(mFilterFilePath);
	QJsonDocument document;
	QJsonArray qjar = document.array();
	int size = mFilterArray.size();
	for (int i = 0; i != size; i++) {
		Filter filter = mFilterArray.at(i);

		QJsonObject objs;
		objs.insert("title", filter.text_title);
		objs.insert("ifand", filter.text_and);
		objs.insert("ifor", filter.text_or);
		objs.insert("ifexclude", filter.text_exclude);
		//qjar.insert(i, objs);
		qjar.append(objs);
	}
	document.setArray(qjar);

	if (file.open(QIODevice::WriteOnly)) {
		file.write(document.toJson(QJsonDocument::Compact));
		file.close();
	}


}
//��д�ļ���Ϣ
void LogAnalyse::readFilterFile()
{
	QFile file(mFilterFilePath);
	QJsonDocument jdc;
	if (file.open(QIODevice::ReadOnly)) {
		QJsonParseError* error = new QJsonParseError;
		jdc = QJsonDocument::fromJson(file.readAll(), error);
		file.close();
	}
	if (jdc.isEmpty()) {
		jdc = QJsonDocument();
	}

	QJsonArray qjar = jdc.array();
	int size = qjar.size();
	for (int i = 0; i < size; i++) {
		QJsonObject qjo = qjar.at(i).toObject();
		QString title = qjo.value("title").toString();
		QString if_and = qjo.value("ifand").toString();
		QString if_or = qjo.value("ifor").toString();
		QString if_exclude = qjo.value("ifexclude").toString();

		Filter filter = Filter(title, if_and, if_or, if_exclude);
		mFilterArray.append(filter); 
	}
 
}

//�����������ListView��
void LogAnalyse::insertFilterToListWidget(int index, Filter& filter)
{
	QListWidgetItem* item = new QListWidgetItem(filter.toString());
	item->setTextAlignment(Qt::AlignLeft);
	item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
	 
	ui.List_Filter->insertItem(index, item); 
}
//�б���˫��
void LogAnalyse::on_List_Filter_itemDoubleClicked(QListWidgetItem* item) {
	int index = ui.List_Filter->row(item);
	Filter  filter = mFilterArray.at(index);

	ui.Edit_IfAnd->setText(filter.text_and);
	ui.Edit_IfOr->setText(filter.text_or);
	ui.Edit_IfExclude->setText(filter.text_exclude);

}


//����������
void LogAnalyse::clear_AnalyseResult(){
	ui.Edit_AnalyzeResult->clear();
	ui.Label_RunInfo->setText(From8Bit("��־����/��ʱ0��"));
	ui.Label_RunNum->setText(From8Bit("��0��"));
}

//��ʼ����
void LogAnalyse::on_BT_Analyze_clicked() {
	QString FilePath = ui.Edit_InputFilePath->text();
	QString If_And = ui.Edit_IfAnd->text();
	QString If_Or = ui.Edit_IfOr->text();
	QString If_Exclude = ui.Edit_IfExclude->text();
	if (FilePath.isEmpty())return;
	analyse(FilePath , If_And , If_Or , If_Exclude);
}


void LogAnalyse::on_analyse_runing(const int& count, const long& elapsed) {
	ui.Label_RunNum->setText(From8Bit("�� %1 ��").arg(count));
	ui.Label_RunInfo->setText(From8Bit("������../��ʱ %1ms").arg(elapsed));
}
void LogAnalyse::on_analyse_end(const int& count, const long& elapsed) {
	ui.Label_RunNum->setText(From8Bit("�� %1 ��").arg(count));
	ui.Label_RunInfo->setText(From8Bit("���/��ʱ %1ms").arg(elapsed));
}
void LogAnalyse::on_analyse_log(const QString& log) {
	ui.Edit_AnalyzeResult->appendHtml(log);
	//ui.label->setText(log);
}
