#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qapplication.h"
#include "qnamespace.h"
#include "qpolygon.h"
#include "qwidget.h"
#include <QMainWindow>
#include <QVector>
#include <QPolygon>
#include <QPainter>
#include <QPaintEvent>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <unistd.h>
#include <QElapsedTimer>
#include <utility>

struct Hexagon : public QPolygon {

    int x_center;
    int y_center;

    Hexagon() {};
    Hexagon (int centerX, int centerY, int size) :
        x_center(centerX), y_center(centerY) {

        for (int i = 0; i < 6; ++i) {
            double angle_rad = M_PI / 3.0 * i;
            int x = x_center + size * cos(angle_rad);
            int y = y_center + size * sin(angle_rad);
            *static_cast<QPolygon*>(this) << QPoint(x, y);
        }
    }
};

enum class cellState {
    EMPTY,
    WALL,
    BOX,
    EXIT,
    UNDEF
};

struct Cell {
    Hexagon hexagon;
    QColor color = Qt::darkYellow;
    cellState state = cellState::EMPTY;

    friend std::ostream& operator<<
    (std::ostream& os, const Cell& obj) {
            os <<
            '(' << obj.hexagon.x_center << ';'
               << obj.hexagon.y_center << ')';
            return os;
    }
};



extern QMap<cellState, QColor> colors;

struct Field : public QWidget {
    QVector<Cell> cells;
    size_t numRows;
    size_t numCols;

    std::pair<size_t, size_t> cur_pos = {0, 0};

    friend std::ostream& operator<<
    (std::ostream& os, const Field& obj) {
        for (auto cell : obj.cells)
            os << cell << ' ';
        return os;
    }

    Field(size_t _numRows, size_t _numCols, size_t cellSize,
             QVector<cellState> &boxes, QWidget *par)
        : QWidget(par), numRows(_numRows), numCols(_numCols) {

        for (size_t j = 0; j < numCols; j++) {
            float odd_offset = (j%2) ? sqrt(3) * (cellSize/2) : 0;
            for (size_t i = 0; i < numRows; i++) {
                Cell cell;
                double offsetX = 1.5 * cellSize;
                double offsetY = sqrt(3) * cellSize;

                cell.hexagon = Hexagon(cellSize + j * offsetX,
                              odd_offset + cellSize + i * offsetY,
                              cellSize);
                cells.append(cell);
            }
        }

        for (int i = cells.size() - 1; i > -1; i--) {
            int y = i/numCols;
            int x = i%numCols;
            int reverse_num = x*numRows + y;
            cells[reverse_num].state = boxes[i];
            cells[reverse_num].color = colors.find(boxes[i]).value();
        }
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        for (const Cell &cell : cells) {
            QPainter painter(this);
            painter.setBrush(cell.color);
            painter.setPen(Qt::black);
            painter.drawPolygon(cell.hexagon);
        }
    };

    size_t buffered_cell_num = 0;
    cellState buffered_state = cellState::EMPTY;

    void keyPressEvent(QKeyEvent *event) override {

        auto tmp_pos = cur_pos;

        if (event->key() == Qt::Key_Q) {
            if (tmp_pos.first > 0) {
                if (!(tmp_pos.first % 2) && tmp_pos.second > 0)
                    tmp_pos.second--;

                tmp_pos.first--;
            }
        }
        else if (event->key() == Qt::Key_E) {
            if (tmp_pos.first < numCols - 1) {
                if (!(tmp_pos.first % 2) && tmp_pos.second > 0)
                    tmp_pos.second--;

                tmp_pos.first++;
            }
        }
        else if (event->key() == Qt::Key_A) {
            if (tmp_pos.first > 0) {
                if (tmp_pos.first % 2 && tmp_pos.second < numRows - 1)
                    tmp_pos.second++;

                tmp_pos.first--;
            }
        }
        else if (event->key() == Qt::Key_D) {
            if (tmp_pos.first < numCols - 1) {
                if (tmp_pos.first % 2 && tmp_pos.second < numRows - 1)
                    tmp_pos.second++;

                tmp_pos.first++;
            }
        }
        else if (event->key() == Qt::Key_W) {
            if (tmp_pos.second > 0)
                tmp_pos.second--;
        }
        else if (event->key() == Qt::Key_S) {
            if (tmp_pos.second < numRows - 1)
                tmp_pos.second++;
        }
        else if (event->key() == Qt::Key_Return) close();
        else return;

        cells[buffered_cell_num].color = colors.find(buffered_state).value();

        int reverse_num = tmp_pos.first*numRows + tmp_pos.second;
        if (cells[reverse_num].state == cellState::WALL) return;

        buffered_state = cells[reverse_num].state;
        buffered_cell_num = reverse_num;

        if (cells[reverse_num].state == cellState::EXIT)
            cells[reverse_num].color = Qt::green;
        else
            cells[reverse_num].color = Qt::black;

        cur_pos = tmp_pos;

        QWidget::update();
    }
};



#endif // MAINWINDOW_H
