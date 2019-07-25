#include "Lifesmaze.h"
#include <QApplication>
#include <QDesktopWidget>

Lifesmaze::Lifesmaze(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	help.setupUi(&dialog);
	model = new Model(this);

	cellSize = 40;
	stopGo = true;

	ui.tableView->setModel(model);
	connect(ui.tableView, SIGNAL(entered(const QModelIndex &)), model, SLOT(selectCells(const QModelIndex &)));
	connect(ui.tableView, SIGNAL(clicked(const QModelIndex &)), model, SLOT(selectStartEnd(const QModelIndex &)));

	connect(ui.Launch, SIGNAL(clicked()), this, SLOT(launch()));
	connect(ui.GoStop, SIGNAL(clicked()), this, SLOT(stop_Go()));
	connect(ui.Refresh, SIGNAL(clicked()), this, SLOT(refresh()));
	
	connect(ui.verticalSlider, SIGNAL(valueChanged(int)), model, SLOT(changeTimerInterval(int)));

	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.actionInfo, SIGNAL(triggered()), &dialog, SLOT(open()));

	QRect rc = QApplication::desktop()->screenGeometry();
	setGeometry(0.1*rc.width(), 0.1*rc.height(),0,0);
	setFixedSize(0.8*rc.width(), 0.81*rc.height());

	show();
	dialog.show();
}

void Lifesmaze::wheelEvent(QWheelEvent * event) {
	if (event->angleDelta().y() > 0) {
		cellSize++;
		ui.tableView->verticalHeader()->setDefaultSectionSize(cellSize);
		ui.tableView->horizontalHeader()->setDefaultSectionSize(cellSize);
	}
	else {
		if (cellSize == 40) return;
		cellSize--;
		ui.tableView->verticalHeader()->setDefaultSectionSize(cellSize);
		ui.tableView->horizontalHeader()->setDefaultSectionSize(cellSize);
	}
}
void Lifesmaze::launch(){
	disconnect(ui.Launch, 0, 0, 0);
	ui.tableView->setAttribute(Qt::WA_TransparentForMouseEvents);
	model->pathComputation();
}

Lifesmaze::~Lifesmaze() { delete model; }

void Lifesmaze::connectBtn() { connect(ui.Launch, SIGNAL(clicked()), this, SLOT(launch())); }

void Lifesmaze::changeTextLable(QString text) { ui.label->setText(text); }

void Lifesmaze::changeTransperent() { ui.tableView->setAttribute(Qt::WA_TransparentForMouseEvents, false); }

void Lifesmaze::stop_Go() {
	if (stopGo) {
		ui.label->setText("Pause");
		model->stop();
		stopGo = false;
	}
	else {
		ui.label->setText("Getting to the finish");
		model->_continue();
		stopGo = true;
	}
}

void Lifesmaze::refresh() { 
	connect(ui.Launch, SIGNAL(clicked()), this, SLOT(launch()));
	changeTextLable("Select start cell");
	model->refresh();
	ui.tableView->setAttribute(Qt::WA_TransparentForMouseEvents, false);
}
