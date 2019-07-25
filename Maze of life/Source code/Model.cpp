#include "Model.h"
#include <QBrush>

Model::Model(Lifesmaze* widg) {
	this->widg = widg;
	finished = false;
	row = 19;
	column = 37;
	field.resize(row);
	field_buf.resize(row);
	field_init.resize(row);
	count.resize(row);
	alr.resize(row);
	rank.resize(row);
	for (int i = 0; i < field.size(); i++) {
		field[i].resize(column);
		field_buf[i].resize(column);
		field_init[i].resize(column);
		count[i].resize(column);
		alr[i].resize(column);
		rank[i].resize(column);
	}
	start = QPoint(-1, -1);
	end = QPoint(-1, -1);
	connect(&timer, SIGNAL(timeout()), this, SLOT(reproduction()));
	timer.setInterval(300);
}

Model::~Model()
{
}

void Model::refresh() {
	timer.stop();
	refreshCommonCode();
	way_reverse.clear();
	vert.clear();
	finished = false;
}

void Model::refreshCommonCode() {
	start = QPoint(-1, -1);
	end = QPoint(-1, -1);
	std::fill(field.begin(), field.end(), QVector<int>(column, 0));
	QModelIndex topLeft = createIndex(0, 0);
	QModelIndex bottomRight = createIndex(row, column);
	emit dataChanged(topLeft, bottomRight);
}

void Model::changeTimerInterval(int value) { timer.setInterval(3000 - 30 * --value); }

void Model::selectCells(const QModelIndex & index) {
	if (start == QPoint(-1, -1) || end == QPoint(-1, -1)) return;
	if (QPoint(index.row(), index.column()) == start || QPoint(index.row(), index.column()) == end) return;

	field[index.row()][index.column()] = 1;
	QModelIndex topLeft = createIndex(0, 0);
	QModelIndex bottomRight = createIndex(18, 21);
	emit dataChanged(topLeft, bottomRight);
}


void Model::selectStartEnd(const QModelIndex & ind) {
	if (start == QPoint(-1, -1)) {
		start = QPoint(ind.row(), ind.column());
		vert.push_back(start);
		vert.push_back(QPoint(-1, -1));
		widg->changeTextLable("Select finish cell");

		QModelIndex topLeft = createIndex(0, 0);
		QModelIndex bottomRight = createIndex(18, 21);
		emit dataChanged(topLeft, bottomRight);
	}
	else if (end == QPoint(-1, -1)) {
		end = QPoint(ind.row(), ind.column());
		widg->changeTextLable("Now, specify alive cells with help Drag-and-drop mouse. How are you ready, push the 'Launch'");

		QModelIndex topLeft = createIndex(0, 0);
		QModelIndex bottomRight = createIndex(18, 21);
		emit dataChanged(topLeft, bottomRight);
	}
}

void Model::pathComputation() {
	field_init = field;
	reachability();
	if (count[end.x()][end.y()] == 0) {
		widg->changeTransperent();
		widg->changeTextLable("No way. Try with new coordinates");
		std::fill(count.begin(), count.end(), QVector<int>(column, 0));
		QTimer::singleShot(100, this, SLOT(refreshCommonCode()));
		widg->connectBtn();
		return;
	}

	dfs(end, false);
	way.clear();
	for (int i(0); i < field.size(); i++) {
		for (int j(0); j < field[i].size(); j++) {
			if (!alr[i][j]) rank[i][j] = 0;
		}
	}
	std::fill(alr.begin(), alr.end(), QVector<bool>(column, false));
	int max(0);
	QPoint part(-1, -1);
	for (int i(0); i < field.size(); i++) {
		for (int j(0); j < field[i].size(); j++) {
			if (rank[i][j] > max) {
				part = QPoint(i, j);
				max = rank[i][j];
			}
		}
	}
	if (part == QPoint(-1, -1)) {
		dfs(end, true);
		std::copy(way.begin(), way.end(), std::front_inserter(way_reverse));
	}
	else {
		QPoint prev_end = end;
		end = part;
		dfs(end, true);
		std::fill(alr.begin(), alr.end(), QVector<bool>(column, false));
		std::fill(count.begin(), count.end(), QVector<int>(column, 0));
		std::copy(way.begin(), way.end(), std::front_inserter(way_reverse));
		way.clear();
		field = field_init;
		for (int i = 0; i < way_reverse.size() - 1; i++) {
			nextGeneration();
			field = field_buf;
		}
		start = part;
		end = prev_end;
		vert.push_back(start);
		vert.push_back(QPoint(-1, -1));
		reachability();
		dfs(end, true);
		std::reverse_copy(way.begin(), way.end() - 1, std::back_inserter(way_reverse));
	}
	std::fill(alr.begin(), alr.end(), QVector<bool>(column, false));
	std::fill(count.begin(), count.end(), QVector<int>(column, 0));
	way.clear();

	field = field_init;
	widg->changeTextLable("Getting to the finish");
	timer.start();
 }

 void Model::reachability() {
	 nextGeneration();
	 field = field_buf;
	 QPoint point;
	 int x, y;

	 while (true) {
		 point = vert.front();
		 vert.pop_front();
		 int x = point.x();
		 int y = point.y();
		 if (x == 0 && y == 0 || x == 0 && y == column - 1 || x == row - 1 && y == 0 || x == row - 1 && y == column - 1) {
			 if (x == 0 && y == 0) {
				 for (int i = x; i - 2 != x; i++) {
					 for (int j = y; j - 2 != y; j++)
						 body_for(i, j, x, y);
				 }
			 }
			 if (x == 0 && y == column - 1) {
				 for (int i = x; i - 2 != x; i++) {
					 for (int j = y; j + 2 != y; j--)
						 body_for(i, j, x, y);
				 }
			 }
			 if (x == row - 1 && y == 0) {
				 for (int i = x; i + 2 != x; i--) {
					 for (int j = y; j - 2 != y; j++)
						 body_for(i, j, x, y);
				 }
			 }
			 if (x == row - 1 && y == column - 1) {
				 for (int i = x; i + 2 != x; i--) {
					 for (int j = y; j + 2 != y; j--)
						 body_for(i, j, x, y);
				 }
			 }
		 }
		 else if (y == 0 || y == column - 1 || x == 0 || x == row - 1) {
			 if (y == 0) {
				 for (int i = x - 1; i - 2 != x; i++) {
					 for (int j = y; j - 2 != y; j++)
						 body_for(i, j, x, y);
				 }
			 }
			 if (y == column - 1) {
				 for (int i = x - 1; i - 2 != x; i++) {
					 for (int j = y; j + 2 != y; j--)
						 body_for(i, j, x, y);
				 }
			 }
			 if (x == 0) {
				 for (int i = x; i - 2 != x; i++) {
					 for (int j = y - 1; j - 2 != y; j++)
						 body_for(i, j, x, y);
				 }
			 }
			 if (x == row - 1) {
				 for (int i = x; i + 2 != x; i--) {
					 for (int j = y - 1; j - 2 != y; j++)
						 body_for(i, j, x, y);
				 }
			 }
		 }
		 else {
			 for (int i = x - 1; i - 2 != x; i++) {
				 for (int j = y - 1; j - 2 != y; j++)
					 body_for(i, j, x, y);
			 }
		 }
		 if (vert.front() == QPoint(-1, -1)) {
			 vert.pop_front();
			 if (vert.isEmpty()) break;
			 nextGeneration();
			 field = field_buf;
			 vert.push_back(QPoint(-1, -1));
		 }
	 }
 }

void Model::body_for(int i, int j, int x, int y) {
	if (i == x && j == y)
		return;
	if (i != x && j != y)
		return;
	if (field[i][j] == 0 && count[i][j] == 0 && QPoint(i, j) != start) {
		if (i < x) {
			if (i != 0 && field[i - 1][j] != 0) return;
			if (j != column - 1 && field[i][j + 1] != 0) return;
			if (j != 0 && field[i][j - 1] != 0) return;
			if (i > 1 && i < row - 1 && j>1 && j < column - 2)
			{
				if (field[i + 1][j - 2] == 1) rank[i][j]++;
				if (field[i][j - 2] == 1) rank[i][j]++;
				if (field[i - 1][j - 1] == 1) rank[i][j]++;
				if (field[i - 2][j] == 1) rank[i][j]++;
				if (field[i - 1][j + 1] == 1) rank[i][j]++;
				if (field[i][j + 2] == 1) rank[i][j]++;
				if (field[i + 1][j + 2] == 1) rank[i][j]++;
			}
		}
		else if (i > x) {
			if (i != row - 1 && field[i + 1][j] != 0) return;
			if (j != column - 1 && field[i][j + 1] != 0) return;
			if (j != 0 && field[i][j - 1] != 0) return;

			if (i > 0 &&
				i < row - 2 &&
				j>1 &&
				j < column - 2)
			{
				if (field[i - 1][j - 2] == 1) rank[i][j]++;
				if (field[i][j - 2] == 1) rank[i][j]++;
				if (field[i + 1][j - 1] == 1) rank[i][j]++;
				if (field[i + 2][j] == 1) rank[i][j]++;
				if (field[i + 1][j + 1] == 1) rank[i][j]++;
				if (field[i][j + 2] == 1) rank[i][j]++;
				if (field[i - 1][j + 2] == 1) rank[i][j]++;
			}
		}
		else if (j < y) {
			if (j != 0 && field[i][j - 1] != 0) return;
			if (i != 0 && field[i - 1][j] != 0) return;
			if (i != row - 1 && field[i + 1][j] != 0) return;

			if (j > 1 && 
				j < column - 1 &&
				i>1 &&
				i < row - 2)
			{
				if (field[i - 2][j + 1] == 1) rank[i][j]++;
				if (field[i - 2][j] == 1) rank[i][j]++;
				if (field[i - 1][j - 1] == 1) rank[i][j]++;
				if (field[i][j - 2] == 1) rank[i][j]++;
				if (field[i + 1][j - 1] == 1) rank[i][j]++;
				if (field[i + 2][j] == 1) rank[i][j]++;
				if (field[i + 2][j + 1] == 1) rank[i][j]++;
			}
		}
		else if (j > y) {
			if (i != 0 && field[i - 1][j] != 0) return;
			if (i != row - 1 && field[i + 1][j] != 0) return;
			if (j != column - 1 && field[i][j + 1] != 0) return;

			if (j > 0 &&
				i > 1 &&
				j < column - 2 &&
				i < row - 2)
			{
				if (field[i - 2][j - 1] == 1) rank[i][j]++;
				if (field[i - 2][j] == 1) rank[i][j]++;
				if (field[i - 1][j + 1] == 1) rank[i][j]++;
				if (field[i][j + 2] == 1) rank[i][j]++;
				if (field[i + 1][j + 1] == 1) rank[i][j]++;
				if (field[i + 2][j] == 1) rank[i][j]++;
				if (field[i + 2][j - 1] == 1) rank[i][j]++;
			}
		}
		count[i][j] = count[x][y] + 1;
		vert.push_back(QPoint(i, j));
	}
}

//void Model::findAllWays() {
//	vert.push_back(end);
//	allWays.push_back(QVector<QPoint>(1, QPoint(end)));
//	do {
//		int x = vert.front().x();
//		int y = vert.front().y();
//		
//		if (x == 0 && y == 0 || x == 0 && y == column - 1 || x == row - 1 && y == 0 || x == row - 1 && y == column - 1) {
//			if (x == 0 && y == 0) {
//				for (int i = x; i - 2 != x; i++) {
//					for (int j = y; j - 2 != y; j++) {
//						if (body_for_dfs(i, j, x, y)) {
//							if (!alr[i][j]) {
//								vert.push_back(QPoint(i, j));
//								alr[i][j] = true;
//							}
//							vspomogat.push_back(QPoint(i, j));
//						}
//					}
//				}
//			}
//			if (x == 0 && y == column - 1) {
//				for (int i = x; i - 2 != x; i++) {
//					for (int j = y; j + 2 != y; j--) {
//						if (body_for_dfs(i, j, x, y)) {
//							if (!alr[i][j]) {
//								vert.push_back(QPoint(i, j));
//								alr[i][j] = true;
//							}
//							vspomogat.push_back(QPoint(i, j));
//						}
//					}
//				}
//			}
//			if (x == row - 1 && y == 0) {
//				for (int i = x; i + 2 != x; i--) {
//					for (int j = y; j - 2 != y; j++) {
//						if (body_for_dfs(i, j, x, y)) {
//							if (!alr[i][j]) {
//								vert.push_back(QPoint(i, j));
//								alr[i][j] = true;
//							}
//							vspomogat.push_back(QPoint(i, j));
//						}
//					}
//				}
//			}
//			if (x == row - 1 && y == column - 1) {
//				for (int i = x; i + 2 != x; i--) {
//					for (int j = y; j + 2 != y; j--) {
//						if (body_for_dfs(i, j, x, y)) {
//							if (!alr[i][j]) {
//								vert.push_back(QPoint(i, j));
//								alr[i][j] = true;
//							}
//							vspomogat.push_back(QPoint(i, j));
//						}
//					}
//				}
//			}
//		}
//		else if (y == 0 || y == column - 1 || x == 0 || x == row - 1) {
//			if (y == 0) {
//				for (int i = x - 1; i - 2 != x; i++) {
//					for (int j = y; j - 2 != y; j++) {
//						if (body_for_dfs(i, j, x, y)) {
//							if (!alr[i][j]) {
//								vert.push_back(QPoint(i, j));
//								alr[i][j] = true;
//							}
//							vspomogat.push_back(QPoint(i, j));
//						}
//					}
//				}
//			}
//			if (y == column - 1) {
//				for (int i = x - 1; i - 2 != x; i++) {
//					for (int j = y; j + 2 != y; j--) {
//						if (body_for_dfs(i, j, x, y)) {
//							if (!alr[i][j]) {
//								vert.push_back(QPoint(i, j));
//								alr[i][j] = true;
//							}
//							vspomogat.push_back(QPoint(i, j));
//						}
//					}
//				}
//			}
//			if (x == 0) {
//				for (int i = x; i - 2 != x; i++) {
//					for (int j = y - 1; j - 2 != y; j++) {
//						if (body_for_dfs(i, j, x, y)) {
//							if (!alr[i][j]) {
//								vert.push_back(QPoint(i, j));
//								alr[i][j] = true;
//							}
//							vspomogat.push_back(QPoint(i, j));
//						}
//					}
//				}
//			}
//			if (x == row - 1) {
//				for (int i = x; i + 2 != x; i--) {
//					for (int j = y - 1; j - 2 != y; j++) {
//						if (body_for_dfs(i, j, x, y)) {
//							if (!alr[i][j]) {
//								vert.push_back(QPoint(i, j));
//								alr[i][j] = true;
//							}
//							vspomogat.push_back(QPoint(i, j));
//						}
//					}
//				}
//			}
//		}
//		else {
//			for (int i = x - 1; i - 2 != x; i++) {
//				for (int j = y - 1; j - 2 != y; j++) {
//					if (body_for_dfs(i, j, x, y)) {
//						if (!alr[i][j]) {
//							vert.push_back(QPoint(i, j));
//							alr[i][j] = true;
//						}
//						vspomogat.push_back(QPoint(i, j));
//					}
//				}
//			}
//		}
//		int size = allWays.size();
//		for (int i = 0; i < size; i++) {
//			if (allWays[i].back() == vert.front()) {
//				int k(0);
//				while (k < vspomogat.size() - 1) {
//					allWays.push_back(allWays[i]);
//					allWays.back().push_back(vspomogat[++k]);
//				}
//				allWays[i].push_back(vspomogat[0]);
//			}
//		}
//		vspomogat.clear();
//		vert.pop_front();
//	}while (vert.front() != start);
//}

bool Model::dfs(QPoint point, bool all) {
	int x = point.x();
	int y = point.y();
	if (alr[x][y]) return false;
	alr[x][y] = true;
	way.push(point);
	if (point == start) return all;
	if (x == 0 && y == 0 || x == 0 && y == column - 1 || x == row - 1 && y == 0 || x == row - 1 && y == column - 1) {
		if (x == 0 && y == 0) {
			for (int i = x; i - 2 != x; i++) {
				for (int j = y; j - 2 != y; j++) {
					if (body_for_dfs(i, j, x, y)) {
						if (dfs(QPoint(i, j), all)) return true;
					}
				}
			}
		}
		if (x == 0 && y == column - 1) {
			for (int i = x; i - 2 != x; i++) {
				for (int j = y; j + 2 != y; j--) {
					if (body_for_dfs(i, j, x, y)) {
						if (dfs(QPoint(i, j), all)) return true;
					}
				}
			}
		}
		if (x == row - 1 && y == 0) {
			for (int i = x; i + 2 != x; i--) {
				for (int j = y; j - 2 != y; j++) {
					if (body_for_dfs(i, j, x, y)) {
						if (dfs(QPoint(i, j), all)) return true;
					}
				}
			}
		}
		if (x == row - 1 && y == column - 1) {
			for (int i = x; i + 2 != x; i--) {
				for (int j = y; j + 2 != y; j--) {
					if (body_for_dfs(i, j, x, y)) {
						if (dfs(QPoint(i, j), all)) return true;
					}
				}
			}
		}
	}
	else if (y == 0 || y == column - 1 || x == 0 || x == row - 1) {
		if (y == 0) {
			for (int i = x - 1; i - 2 != x; i++) {
				for (int j = y; j - 2 != y; j++) {
					if (body_for_dfs(i, j, x, y)) {
						if (dfs(QPoint(i, j), all)) return true;
					}
				}
			}
		}
		if (y == column - 1) {
			for (int i = x - 1; i - 2 != x; i++) {
				for (int j = y; j + 2 != y; j--) {
					if (body_for_dfs(i, j, x, y)) {
						if (dfs(QPoint(i, j), all)) return true;
					}
				}
			}
		}
		if (x == 0) {
			for (int i = x; i - 2 != x; i++) {
				for (int j = y - 1; j - 2 != y; j++) {
					if (body_for_dfs(i, j, x, y)) {
						if (dfs(QPoint(i, j), all)) return true;
					}
				}
			}
		}
		if (x == row - 1) {
			for (int i = x; i + 2 != x; i--) {
				for (int j = y - 1; j - 2 != y; j++) {
					if (body_for_dfs(i, j, x, y)) {
						if (dfs(QPoint(i, j), all)) return true;
					}
				}
			}
		}
	}
	else {
		for (int i = x - 1; i - 2 != x; i++) {
			for (int j = y - 1; j - 2 != y; j++) {
				if (body_for_dfs(i, j, x, y)) {
					if (dfs(QPoint(i, j), all)) return true;
				}
			}
		}
	}
	way.pop();
	return false;
}

bool Model::body_for_dfs(int i, int j, int x, int y) {
	if (i == x && j == y) return false;
	if (i != x && j != y) return false;
	if (count[i][j] == 0 && QPoint(i, j) != start) return false;
	if (count[x][y] - 1 != count[i][j]) return false;
	return true;
}

void Model::startTimer() {
	connect(&timer, SIGNAL(timeout()), this, SLOT(reproduction()));
	timer.start(500);
}

void Model::stop() { timer.stop(); }

void Model::_continue() { timer.start(); }

void Model::reproduction() {
	nextGeneration();
	//if (field_prev == field_buf) qDebug() << "REPEAT";
	//for (int i = 0; i < field.size(); i++) {
	//	for (int j = 0; j < field[i].size(); j++) 
	//		field_prev[i][j] = field[i][j];
	//}
	field = field_buf;
	if (way_reverse.size() != 0) {
		prev = way_reverse.front();
		way_reverse.pop_front();
	}
	if (way_reverse.size() == 0) {
		widg->changeTextLable("FINISH!");
		finished = true;
	}
	QModelIndex topLeft = createIndex(0, 0);
	QModelIndex bottomRight = createIndex(row, column);
	emit dataChanged(topLeft, bottomRight);
}

int Model::rowCount(const QModelIndex & parent) const { return row; }

int Model::columnCount(const QModelIndex & parent) const { return column; }

Qt::ItemFlags Model::flags(const QModelIndex & index) const { return Qt::NoItemFlags; }

QVariant Model::data(const QModelIndex & index, int role) const {
	if (role == Qt::TextAlignmentRole) return Qt::AlignCenter;
	if (role == Qt::FontRole) {
		if (QPoint(index.row(), index.column()) == end) return QFont("Times New Roman", 20);
	}
	if (role == Qt::DecorationRole) {
		if (!finished && QPoint(index.row(), index.column()) == end) return QPixmap("finish.png");
		if (way_reverse.size() != 0) {
			if (QPoint(index.row(), index.column()) == way_reverse.front()) {
				int i = way_reverse.front().x();
				int j = way_reverse.front().y();
				int x = prev.x();
				int y = prev.y();

				if (i > x) return QPixmap("DownArrow.png");
				else if (i < x) return QPixmap("UpArrow.png");
				else if (j < y) return QPixmap("LeftArrow.png");
				else if (j > y) return QPixmap("RightArrow.png");
			} 
		}
	}
	if (role == Qt::BackgroundRole) {
		if (!finished && way_reverse.size() == 0) {
			if (QPoint(index.row(), index.column()) == start) return QBrush(Qt::black);
		}
		if (field[index.row()][index.column()] == 1) return QBrush(Qt::yellow);
		else return  QBrush(Qt::gray);
	}
	return QVariant();
}

void Model::nextGeneration() {
	//for (int i = 0; i <field.size(); i++) {
	//	for (int j = 0; j < field[i].size(); j++) 
	//		field_buf[i][j] = 0;
	//}
	std::fill(field_buf.begin(), field_buf.end(), QVector<int>(column, 0));
	for (int i = 0; i < field.size(); i++) {
		for (int j = 0; j < field[i].size(); j++) {
			if (field[i][j] == 2) {
				field_buf[i][j] = 2;
				continue;
			}
			int k = countAliveAround(i, j);
			if (field[i][j]) {
				if (k == 2 || k == 3) field_buf[i][j] = 1; 
			}
			else {
				if (k == 3) field_buf[i][j] = 1;
			}
		}
	}
}

int Model::countAliveAround(int x, int y) {
	int k = 0;
	if (x == 0 && y == 0 || x == 0 && y == column - 1 || x == row - 1 && y == 0 || x == row - 1 && y == column - 1) {
		if (x == 0 && y == 0) {
			for (int i = x; i - 2 != x; i++) {
				for (int j = y; j - 2 != y; j++)
					if (aliveBody(i, j, x, y)) k++;
			}
		}
		if (x == 0 && y == column - 1) {
			for (int i = x; i - 2 != x; i++) {
				for (int j = y; j + 2 != y; j--)
					if (aliveBody(i, j, x, y)) k++;
			}
		}
		if (x == row - 1 && y == 0) {
			for (int i = x; i + 2 != x; i--) {
				for (int j = y; j - 2 != y; j++)
					if (aliveBody(i, j, x, y)) k++;
			}
		}
		if (x == row - 1 && y == column - 1) {
			for (int i = x; i + 2 != x; i--) {
				for (int j = y; j + 2 != y; j--)
					if (aliveBody(i, j, x, y)) k++;
			}
		}
	}
	else if (y == 0 || y == column - 1 || x == 0 || x == row - 1) {
		if (y == 0) {
			for (int i = x - 1; i - 2 != x; i++) {
				for (int j = y; j - 2 != y; j++)
					if (aliveBody(i, j, x, y)) k++;
			}
		}
		if (y == column - 1) {
			for (int i = x - 1; i - 2 != x; i++) {
				for (int j = y; j + 2 != y; j--)
					if (aliveBody(i, j, x, y)) k++;
			}
		}
		if (x == 0) {
			for (int i = x; i - 2 != x; i++) {
				for (int j = y - 1; j - 2 != y; j++)
					if (aliveBody(i, j, x, y)) k++;
			}

		}
		if (x == row - 1) {
			for (int i = x; i + 2 != x; i--) {
				for (int j = y - 1; j - 2 != y; j++)
					if (aliveBody(i, j, x, y)) k++;
			}
		}
	}
	else {
		for (int i = x - 1; i - 2 != x; i++) {
			for (int j = y - 1; j - 2 != y; j++)
				if (aliveBody(i, j, x, y)) k++;
		}
	}
	return k;
}

bool Model::aliveBody(int i, int j, int x, int y) {
	if (i == x && j == y) return 0;
	if (field[i][j] == 1) return 1;
	else return 0;
}



