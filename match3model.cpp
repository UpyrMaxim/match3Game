#include "match3model.h"

#define MIN_MATCH 3

Match3Model::Match3Model(QObject *parent, const int dimentionX, const int dimentionY)
    : QAbstractListModel(parent), m_moveCounter(0),  m_score(0), m_selectedSellIndex(-1), m_dimentionX(dimentionX), m_dimentionY(dimentionY)
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

void Match3Model::resetGame()
{
        generateCells();
        removeAllMatches();

        m_score = 0;
        emit scoreChanged();

        m_moveCounter = 0;
        emit moveCounterChanged();

        m_selectedSellIndex = -1;
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
      m_dimentionY = board["height"].toInt();
      m_dimentionX = board["width"].toInt();
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
    endRemoveRows();

    m_cells[col].erase(m_cells[col].begin() + row);

    beginInsertRows(QModelIndex(), col * m_dimentionY, col * m_dimentionY);
    m_cells[col].push_front(getRandomCellColorId());
    endInsertRows();

    increaseScore(addToScore);
//    auto newIndex = createIndex(index,0);
//    emit dataChanged(newIndex, newIndex, {Qt::DecorationRole});

}

void Match3Model::removeAllMatches()
{
    beginResetModel();

    bool dataChanged = true;
    while (dataChanged) {
        auto cellsToRemove = checkBoardCells();
        if (cellsToRemove.size()) {
            for (const auto &index : cellsToRemove) {
                int col =  index / m_dimentionY;
                int row = index % m_dimentionY;
                removeElement(col, row, 0);
            }
        } else {
            dataChanged = false;
        }
    }

    endResetModel();
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

QList<int> Match3Model::swapCells(int sourceIndex, int targetIndex, bool reSwap)
{
    if (sourceIndex == targetIndex || sourceIndex < 0 || targetIndex < 0) {
        return QList<int>();
    }

    int sourceCol =  sourceIndex / m_dimentionY;
    int sourceRow = sourceIndex % m_dimentionY;

    int targetCol =  targetIndex / m_dimentionY;
    int targetRow = targetIndex % m_dimentionY;

    int shift = sourceIndex > targetIndex ? 0 : 1;

    beginMoveRows(QModelIndex(), sourceIndex, sourceIndex, QModelIndex(), targetIndex + shift);
    endMoveRows();

    if (abs(sourceIndex - targetIndex) > 1) {
        int zeroPositionShift = targetIndex > sourceIndex ? -1 : 1;
        beginMoveRows(QModelIndex(), targetIndex + zeroPositionShift, targetIndex + zeroPositionShift, QModelIndex(), sourceIndex + shift + zeroPositionShift);
        endMoveRows();
    }

    swap(m_cells[sourceCol][sourceRow], m_cells[targetCol][targetRow]);
    if (reSwap) {
        return QList<int>();
    }

    increaseMoveCounter();
    return checkBoardCells();
}

QList<int> Match3Model::reSwapCells(int sourceIndex, int targetIndex)
{
    return swapCells(sourceIndex, targetIndex, true);
}

void Match3Model::removeCell(int index)
{
    int col =  index / m_dimentionY;
    int row = index % m_dimentionY;

    removeElement(col, row, 0);
}

QList<int> Match3Model::checkBoardCells()
{
    typedef QVector<int> inner;
    QVector<inner> checkRows(m_dimentionX, inner(m_dimentionY));
    QVector<inner> checkCols(m_dimentionX, inner(m_dimentionY));
    QList<int> removeCells;


    for(int col = 0; col < m_dimentionX; ++col) {
        checkCol(checkCols, col);
    }
     for(int row = 0; row < m_dimentionY; ++row) {
        checkRow(checkRows, row);
    }

    for (int col = 0; col < m_dimentionX; ++col) {
          for (int row = 0; row < m_dimentionY; ++row) {
              if ( max(checkCols[col][row],checkRows[col][row]) >= MIN_MATCH ) {
                  removeCells.push_back(col * m_dimentionY + row);
              }
          }
    }

    return removeCells;

}


int Match3Model::checkCol(QVector<QVector<int> > &cells, int col, int row,int value)
{
    bool equal;
    if (row >= m_dimentionY - 1) {
        equal = m_cells[col][row] == m_cells[col][row - 1];
        return value + equal;
    }

    equal = m_cells[col][row] == m_cells[col][row + 1];
    int newValue = equal ? value + 1 : 1;
    int v = checkCol(cells, col, row + 1, newValue);

    if (equal) {
        value = v;
    }

    cells[col][row] = value;

    return value;
}

int Match3Model::checkRow(QVector<QVector<int> > &cells, int row, int col, int value)
{
    bool equal;
    if (col >= m_dimentionX - 1) {
        equal = m_cells[col][row] == m_cells[col - 1][row];
        return value + equal;
    }

    equal = m_cells[col][row] == m_cells[col + 1][row];
    int newValue = equal ? value + 1 : 1;
    int v = checkRow(cells, row, col + 1, newValue);

    if (equal) {
        value = v;
    }

    cells[col][row] = value;

    return value;
}
