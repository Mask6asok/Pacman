

#include "astart.h"
#include <QDebug>

int Astar::calcG(Point *temp_start, Point *point)
{
    int extraG = (abs(point->x - temp_start->x) + abs(point->y - temp_start->y)) == 1 ? kCost1 : kCost2;
    int parentG = point->parent == NULL ? 0 : point->parent->G; //如果是初始节点，则其父节点是空
    return parentG + extraG;
}

int Astar::calcH(Point *point, Point *end)
{
    int xOffset = point->x - end->x;
    int yOffset = point->y - end->y;
    xOffset > 0 ? xOffset : -xOffset;
    yOffset > 0 ? yOffset : -yOffset;
    return xOffset + yOffset;
}

int Astar::calcF(Point *point)
{
    return point->G + point->H;
}

Point *Astar::getLeastFpoint()
{
    if (!openList.empty())
    {
        auto resPoint = openList.front();
        for (auto &point : openList)
            if (point->F < resPoint->F)
            {
                resPoint = point;
            }
        return resPoint;
    }
    return NULL;
}

Point *Astar::findPath(Point &startPoint, Point &endPoint, bool isIgnoreCorner)
{
    openList.push_back(new Point(startPoint.x, startPoint.y)); //置入起点,拷贝开辟一个节点，内外隔离
    while (!openList.empty())
    {
        auto curPoint = getLeastFpoint(); //找到F值最小的点
        openList.remove(curPoint);        //从开启列表中删除
        closeList.push_back(curPoint);    //放到关闭列表

        auto surroundPoints = getSurroundPoints(curPoint, isIgnoreCorner);
        for (auto &target : surroundPoints)
        {
            //2,对某一个格子，如果它不在开启列表中，加入到开启列表，设置当前格为其父节点，计算F G H
            if (!isInList(openList, target))
            {
                target->parent = curPoint;

                target->G = calcG(curPoint, target);
                target->H = calcH(target, &endPoint);
                target->F = calcF(target);

                openList.push_back(target);
            }
            //3，对某一个格子，它在开启列表中，计算G值, 如果比原来的大, 就什么都不做, 否则设置它的父节点为当前点,并更新G和F
            else
            {
                int tempG = calcG(curPoint, target);
                if (tempG < target->G)
                {
                    target->parent = curPoint;

                    target->G = tempG;
                    target->F = calcF(target);
                }
            }
            Point *resPoint = isInList(openList, &endPoint);
            if (resPoint)
            {
                return resPoint;
            } //返回列表里的节点指针，不要用原来传入的endpoint指针，因为发生了深拷贝
        }
    }

    return NULL;
}



void Astar::GetPath(Point &startPoint, Point &endPoint, std::list<Point *> &path)
{
    Point *result = findPath(startPoint, endPoint, false);
    while (result)
    {
        path.push_front(result);
        result = result->parent;
    }
    if (path.size() != 0)
    {
        //qDebug()<<"";
        path.pop_front();
    }
    else
    {
        qDebug() << "no path";
    }

    openList.clear();
    closeList.clear();
}

Point *Astar::isInList(const std::list<Point *> &list, const Point *point) const
{
    //判断某个节点是否在列表中，这里不能比较指针，因为每次加入列表是新开辟的节点，只能比较坐标
    for (auto p : list)
        if (p->x == point->x && p->y == point->y)
        {
            return p;
        }
    return NULL;
}

bool Astar::isCanreach(const Point *point, const Point *target, bool isIgnoreCorner) const
{
    if (target->x < 0 || target->x > maze.size() - 1 || target->y < 0 || target->y > maze[0].size() - 1 || maze[target->x][target->y] == 1 || target->x == point->x && target->y == point->y || isInList(closeList, target)) //如果点与当前节点重合、超出地图、是障碍物、或者在关闭列表中，返回false
    {
        return false;
    }
    else
    {
        if (abs(point->x - target->x) + abs(point->y - target->y) == 1) //非斜角可以
        {
            return true;
        }
    }
}

std::vector<Point *> Astar::getSurroundPoints(const Point *point, bool isIgnoreCorner) const
{
    std::vector<Point *> surroundPoints;

    int x = point->x;
    int y = point->y;
    if (isCanreach(point, new Point(x - 1, y), isIgnoreCorner))
    {
        surroundPoints.push_back(new Point(x - 1, y));
    }
    if (isCanreach(point, new Point(x, y + 1), isIgnoreCorner))
    {
        surroundPoints.push_back(new Point(x, y + 1));
    }
    if (isCanreach(point, new Point(x + 1, y), isIgnoreCorner))
    {
        surroundPoints.push_back(new Point(x + 1, y));
    }
    if (isCanreach(point, new Point(x, y - 1), isIgnoreCorner))
    {
        surroundPoints.push_back(new Point(x, y - 1));
    }
    return surroundPoints;
}
