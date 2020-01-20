#include "match3model.h"

Match3Model::Match3Model(QObject *parent, const int dimentionX, const int dimentionY)
    : QAbstractListModel(parent), moveCounter(0),  score(0), selectedSellIndex(-1), dimentionX(dimentionX), dimentionY(dimentionY)
{
    srand (time(NULL));

    initByJson();
    resetGame();
}

int Match3Model::rowCount(const QModelIndex &) const
{
    return dimentionX * dimentionY; // data reprented by 2d array
}

QVariant Match3Model::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    int col = index.row() / dimentionY;
    int row = index.row() % dimentionY;

    if (role == Qt::DecorationRole) {
        QString colorName = cellTypes[ cells[col][row] ].toString();
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
    return dimentionX;
}

int Match3Model::getDimentionY() const
{
    return dimentionY;
}

int Match3Model::getScore() const
{
    return score;
}

int Match3Model::getMoveCounter() const
{
    return moveCounter;
}

void Match3Model::resetGame()
{
        beginResetModel();

        cells.clear();
        generateCells();

        endResetModel();

        score = 0;
        emit scoreChanged();

        moveCounter = 0;
        emit moveCounterChanged();

        selectedSellIndex = -1;
}


void Match3Model::initByJson()
{
      Q_INIT_RESOURCE(qml);

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
      dimentionY = board["height"].toString().toUInt();
      dimentionX = board["width"].toString().toUInt();
      // set cells types
      cellTypes = jsonObject.value(QString("itemTypes")).toArray();
}

void Match3Model::generateCells()
{
    bool dataChanged = true;

    beginResetModel();
    cells.clear();

    for (int col = 0; col < dimentionX; ++col) {
        deque<int> tmp_vector;

        for (int row = 0; row < dimentionY; ++row) {
            tmp_vector.push_back(getRandomCellColorId());
        }

        cells.push_back(tmp_vector);
    }

    while (dataChanged) {
        auto cellsToRemove = checkBoard();
        if (cellsToRemove.size()) {
            for (const auto &index : cellsToRemove) {
                int col =  index / dimentionY;
                int row = index % dimentionY;
                removeElement(col, row, 0);
            }
        } else {
            dataChanged = false;
        }
    }

    endResetModel();
}


void Match3Model::removeElement(int col, int row, int addToScore)
{
    int index = col * dimentionY + row;
    beginRemoveRows(QModelIndex(), index, index);
    endRemoveRows();

    cells[col].erase(cells[col].begin() + row);

    beginInsertRows(QModelIndex(), col * dimentionY, col * dimentionY);
    cells[col].push_front(getRandomCellColorId());
    endInsertRows();

    increaseScore(addToScore);
//    auto newIndex = createIndex(index,0);
//    emit dataChanged(newIndex, newIndex, {Qt::DecorationRole});

}

int Match3Model::getRandomCellColorId()
{
    return rand() % cellTypes.size();
}

void Match3Model::increaseScore(int multiplicator)
{
    score += 10 + 2 * multiplicator;
    emit scoreChanged();
}

void Match3Model::increaseMoveCounter()
{
    moveCounter++;
    emit moveCounterChanged();
}

QList<int> Match3Model::swapCells(int sourceIndex, int targetIndex, bool reSwap)
{
    if (sourceIndex == targetIndex || sourceIndex < 0 || targetIndex < 0) {
        return QList<int>();
    }

    int sourceCol =  sourceIndex / dimentionY;
    int sourceRow = sourceIndex % dimentionY;

    int targetCol =  targetIndex / dimentionY;
    int targetRow = targetIndex % dimentionY;

    int shift = sourceIndex > targetIndex ? 0 : 1;

    beginMoveRows(QModelIndex(), sourceIndex, sourceIndex, QModelIndex(), targetIndex + shift);
    endMoveRows();

    if (abs(sourceIndex - targetIndex) > 1) {
        int zeroPositionShift = targetIndex > sourceIndex ? -1 : 1;
        beginMoveRows(QModelIndex(), targetIndex + zeroPositionShift, targetIndex + zeroPositionShift, QModelIndex(), sourceIndex + shift + zeroPositionShift);
        endMoveRows();
    }

    swap(cells[sourceCol][sourceRow], cells[targetCol][targetRow]);
    if (reSwap) {
        return QList<int>();
    }

    increaseMoveCounter();
    return findCellsToRemove(sourceCol, sourceRow) + findCellsToRemove(targetCol, targetRow);
}


QList<int> Match3Model::findCellsToRemove(int x, int y)
{
    queue<pair<int, int>> checkQuery;
    int matchArray[dimentionX * dimentionY];
    fill_n(matchArray, dimentionX * dimentionY, 0);

    checkQuery.push(pair<int, int>(x, y));

    while (!checkQuery.empty()) {
        int checkedX = checkQuery.front().first;
        int checkedY = checkQuery.front().second;
        if (cells[x][y] == cells[checkedX][checkedY] && !matchArray[checkedX * dimentionY + checkedY]) {
            matchArray[checkedX * dimentionY + checkedY] = 1;

            if (checkedX > 0) {
                checkQuery.push(pair<int, int>(checkedX - 1, checkedY));
            }
            if (checkedX < dimentionX - 1) {
                checkQuery.push(pair<int, int>(checkedX + 1, checkedY));
            }
            if (checkedY > 0) {
                checkQuery.push(pair<int, int>(checkedX, checkedY - 1));
            }
            if (checkedY < dimentionY - 1) {
                checkQuery.push(pair<int, int>(checkedX, checkedY + 1));
            }
        }
        checkQuery.pop();
    }

    return findMatch3Items(matchArray);
}


QList<int> Match3Model::findMatch3Items(int *boardCells)
{
    bool shuoldBeDeleted = false;
    int elementsMatch;
    QList<int> cellsToRemove;
    // check rows
    for (int col = 0; col < dimentionX; ++col) {
        elementsMatch  = 0;
        for (int row = 0; row < dimentionY; ++row) {
            int cellIndex = col * dimentionY + row;
            if (boardCells[cellIndex]) {
                elementsMatch++;
                cellsToRemove.push_back(cellIndex);
                if (elementsMatch >= 3) {
                    shuoldBeDeleted = true;
                }
            }
        }
    }

    if (!shuoldBeDeleted ) {
        // check cols
        for (int row = 0; row < dimentionY; ++row) {
            elementsMatch  = 0;
            for (int col = 0; col < dimentionX; ++col) {
               if (boardCells[col * dimentionY + row]) {
                    elementsMatch++;

                    if (elementsMatch >= 3) {
                        shuoldBeDeleted = true;
                        // to exit
                        col = dimentionX;
                        row = dimentionY;
                    }
                }
            }
        }
    }

    if (shuoldBeDeleted) {
        return cellsToRemove;
    }

    return QList<int>();
}



QList<int> Match3Model::reSwapCells(int sourceIndex, int targetIndex)
{
    return swapCells(sourceIndex, targetIndex, true);
}

void Match3Model::removeCell(int index)
{
    int col =  index / dimentionY;
    int row = index % dimentionY;

    removeElement(col, row, 0);
}

QList<int> Match3Model::checkBoard()
{
    QList<int> cellsList;
    for (int col = 0; col < dimentionX; ++col) {
        for (int row = 0; row < dimentionY; ++row) {
            cellsList += findCellsToRemove(col, row);
        }
    }

    return cellsList.toSet().toList(); // its make it unique
}
