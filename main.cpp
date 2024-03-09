#include "mainwindow.h"
#include "qmap.h"
#include "qnamespace.h"

#include <QApplication>
#include <cstddef>
#include <iostream>
#include <utility>

QMap<cellState, QColor> colors = {
    {cellState::EMPTY, Qt::darkMagenta},
    {cellState::BOX, Qt::red},
    {cellState::WALL, Qt::darkBlue},
    {cellState::EXIT, Qt::white},
    {cellState::UNDEF, Qt::gray}
};

using cellState::BOX;
using cellState::EMPTY;
using cellState::EXIT;
using cellState::WALL;
using cellState::UNDEF;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    size_t width = 6;
    size_t height = 5;
    size_t size = 30;

    QVector<cellState> boxes = {
        EMPTY, WALL, EMPTY, EMPTY, WALL,  WALL,
        EMPTY, BOX, EMPTY, EMPTY, EMPTY, WALL,
        EMPTY, EMPTY, WALL, EXIT, WALL,  WALL,
        WALL, WALL, EMPTY, EMPTY, EMPTY, WALL,
        EMPTY, WALL, BOX, WALL, UNDEF,  WALL
    };

    Field f(height, width, size, boxes, nullptr);
    f.show();

    return a.exec();
}
