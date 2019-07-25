#pragma once

#include <QtWidgets/QWidget>
#include "ui_mainWindow.h"
#include "ui_Help.h"
#include "Model.h"
#include <QTableView>
#include <QWheelEvent>
#include <QMainWindow>
#include <QDialog>

class Model;
class Lifesmaze : public QMainWindow
{
	Q_OBJECT
		
public:
	Lifesmaze(QWidget *parent = Q_NULLPTR);
	~Lifesmaze();
	void changeTextLable(QString text);
	void changeTransperent();
	void wheelEvent(QWheelEvent *event);
	void connectBtn();
private:
	bool stopGo;
	int cellSize;
	Model* model;
	QDialog dialog;
	Ui::MainWindow ui;
	Ui::Dialog help;
private slots:
	void launch();
	void stop_Go();
	void refresh();
};
