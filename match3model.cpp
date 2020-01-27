#include "match3model.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>

#include <QColor>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include <QDebug>

Match3Model::Match3Model(QObject *parent, const int dimentionX, const int dimentionY)
    : QAbstractListModel(parent), m_moveCounter(0),  m_score(0), m_selectedIndex(-1), m_dimentionX(dimentionX), m_dimentionY(dimentionY)
{
    srand (time(nullptr));

    initByJson();
    resetGame();
}

int Match3Model::rowCount(const QModelIndex &) const
{
    return m_dimentionX * m_dimentionY; // data reprented by 2d array
}

QVariant Match3Model::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    int col = index.row() / m_dimentionY;
    int row = index.row() % m_dimentionY;

    if (role == Qt::DecorationRole) {
        QString colorName = m_cellTypes[ m_cells[col][row] ].toString();
        return QColor(colorName);
    } else {
        return QVariant();
    }
}

Qt::ItemFlags Match3Model::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::ItemIsEnabled;
    }

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

int Match3Model::getDimentionX() const
{
    return m_dimentionX;
}

int Match3Model::getDimentionY() const
{
    return m_dimentionY;
}

int Match3Model::getScore() const
{
    return m_score;
}

int Match3Model::getMoveCounter() const
{
    return m_moveCounter;
}

int Match3Model::getSelected() const
{
    return m_selectedIndex;
}

void Match3Model::resetGame()
{
    beginResetModel();

    generateCells();
    removeAllMatches();

    m_score = 0;
    emit scoreChanged();

    m_moveCounter = 0;
    emit moveCounterChanged();

    endResetModel();
}


void Match3Model::initByJson()
{
    QString val;
    QFile file;
    file.setFileName(":/setting.json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      qWarning() << "Setting file is not exists";
      return;
    }
    val = file.readAll();
    file.close();

    // get json content
    QJsonDocument document = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject jsonObject = document.object();
    QJsonObject board = jsonObject.value(QString("board")).toObject();

    // set dimention values
    m_dimentionY = minMatch > board["height"].toInt() ? minMatch : board["height"].toInt();
    m_dimentionX = minMatch > board["width"].toInt() ?  minMatch : board["width"].toInt();
    // set cells types
    m_cellTypes = jsonObject.value(QString("itemTypes")).toArray();
}

void Match3Model::generateCells()
{
    m_cells.clear();

    for (int col = 0; col < m_dimentionX; ++col) {
        QList<int> tmp_vector;

        for (int row = 0; row < m_dimentionY; ++row) {
            tmp_vector.push_back(getRandomCellColorId());
        }

        m_cells.push_back(tmp_vector);
    }
}


void Match3Model::removeElement(int col, int row, int addToScore)
{
    int index = col * m_dimentionY + row;
    beginRemoveRows(QModelIndex(), index, index);
    m_cells[col].removeAt(row);
    endRemoveRows();

    beginInsertRows(QModelIndex(), col * m_dimentionY, col * m_dimentionY);
    m_cells[col].push_front(getRandomCellColorId());
    endInsertRows();

    increaseScore(addToScore);
}

void Match3Model::setSelectedIndex(int index)
{
    m_selectedIndex = index;
    emit selectedIndexChanged();
}

void Match3Model::removeMatches()
{
    for (const auto &index : m_cellsToRemove) {
        int col =  index / m_dimentionY;
        int row = index % m_dimentionY;
        removeElement(col, row, 0);
    }
}

void Match3Model::removeAllMatches()
{
    bool dataChanged = true;
    while (dataChanged) {
        checkBoardCells();
        if (m_cellsToRemove.size()) {
            removeMatches();
            m_cellsToRemove.clear();
        } else {
            dataChanged = false;
        }
    }
}

int Match3Model::getRandomCellColorId()
{
    return rand() % m_cellTypes.size();
}

void Match3Model::increaseScore(int multiplicator)
{
    m_score += 10 + 2 * multiplicator;
    emit scoreChanged();
}

void Match3Model::increaseMoveCounter()
{
    m_moveCounter++;
    emit moveCounterChanged();
}

bool Match3Model::chooseCell(int index)
{
    if (m_selectedIndex < 0) {
        setSelectedIndex(index);
        return true;
    }

    if (index == m_selectedIndex) {
        setSelectedIndex();
        return true;
    }

    int indexesDifference = abs(index - m_selectedIndex);
    bool swapIsValid = indexesDifference == 1 || indexesDifference == m_dimentionY;

    if (!swapIsValid) {
        setSelectedIndex(index);
        return true;
    }

    int sourceCol =  index / m_dimentionY;
    int sourceRow = index % m_dimentionY;
    int targetCol =  m_selectedIndex / m_dimentionY;
    int targetRow = m_selectedIndex % m_dimentionY;

    swap(m_cells[sourceCol][sourceRow], m_cells[targetCol][targetRow]);
    checkBoardCells();

    if (!m_cellsToRemove.size()) {
        swap(m_cells[sourceCol][sourceRow], m_cells[targetCol][targetRow]);
        setSelectedIndex();
        return false;
    }

    moveCells(index);
    increaseMoveCounter();
    setSelectedIndex();

    return true;
}

void Match3Model::moveCells(int index)
{
    int shift = index > m_selectedIndex ? 0 : 1;

    beginMoveRows(QModelIndex(), index, index, QModelIndex(), m_selectedIndex + shift);
    endMoveRows();

    if (abs(index - m_selectedIndex) > 1) {
        int zeroPositionShift = m_selectedIndex > index ? -1 : 1;
        beginMoveRows(QModelIndex(), m_selectedIndex + zeroPositionShift, m_selectedIndex + zeroPositionShift, QModelIndex(), index + shift + zeroPositionShift);
        endMoveRows();
    }
}

void Match3Model::removeCells()
{

    if (!m_cellsToRemove.size()) {
        return;
    }
    removeMatches();
    checkBoardCells();
}

void Match3Model::checkBoardCells()
{
    typedef QVector<int> Inner;
    QVector<Inner> checkRows(m_dimentionX, Inner(m_dimentionY));
    QVector<Inner> checkCols(m_dimentionX, Inner(m_dimentionY));


    for(int col = 0; col < m_dimentionX; ++col) {
        checkCol(checkCols, col, 0);
    }
     for(int row = 0; row < m_dimentionY; ++row) {
        checkRow(checkRows, 0, row);
    }

    m_cellsToRemove.clear();

    for (int col = 0; col < m_dimentionX; ++col) {
          for (int row = 0; row < m_dimentionY; ++row) {
              if ( max(checkCols[col][row], checkRows[col][row]) >= minMatch ) {
                  m_cellsToRemove.push_back(col * m_dimentionY + row);
              }
          }
    }
}


int Match3Model::checkCol(QVector<QVector<int> > &cells, int col, int row,int value)
{
    if (row == m_dimentionY - 1) {
        cells[col][row] = value;
        return value;
    }

    bool equal = m_cells[col][row] == m_cells[col][row + 1];
    int newValue = equal ? value + 1 : 1;
    int matches = checkCol(cells, col, row + 1, newValue);

    if (equal) {
        value = matches;
    }

    cells[col][row] = value;

    return value;
}

int Match3Model::checkRow(QVector<QVector<int> > &cells, int col, int row, int value)
{
    if (col == m_dimentionX - 1) {
        cells[col][row] = value;
        return value;
    }

    bool equal = m_cells[col][row] == m_cells[col + 1][row];
    int newValue = equal ? value + 1 : 1;
    int matches = checkRow(cells, col + 1, row, newValue);

    if (equal) {
        value = matches;
    }

    cells[col][row] = value;

    return value;
}
