#pragma once

#include <QAbstractTableModel>
#include <QTimer>
#include <QQueue>
#include <QStack>
#include "Lifesmaze.h"
class Lifesmaze;
class Model : public QAbstractTableModel
{
	Q_OBJECT
private:
	int row, column;
	int max;
	bool finished;
	QVector<QVector<int>> field;
	QVector<QVector<int>> field_init;
	QVector<QVector<int>> field_buf;
	QVector<QVector<int>> count;
	QVector<QPoint> way_reverse;
	QTimer timer;
	QPoint start, end;
	QPoint prev;
	QQueue<QPoint> vert;
	QStack<QPoint> way;
	QVector<QVector<bool>> alr;
	QVector<QVector<int>> rank;
	
	Lifesmaze* widg;

	void nextGeneration();
	int countAliveAround(int x, int y);
	bool aliveBody(int i, int j, int x, int y);
	void body_for(int i, int j, int x, int y);
	bool dfs(QPoint point, bool all);
	//void findAllWays();
	bool body_for_dfs(int i, int j, int x, int y);
	void reachability();

public:
	Model(Lifesmaze* widg);
	~Model();
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	void startTimer();
	void stop();
	void _continue();
public slots:
	void selectCells(const QModelIndex & ind);
	void selectStartEnd(const QModelIndex & ind);
	void changeTimerInterval(int value);
	void pathComputation();
	void refresh();
private slots:
	void reproduction();
	void refreshCommonCode();
};
