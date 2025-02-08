// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "chessitem.h"
#include "constants.h"

#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
#include <QtDebug>
#include <QImageReader>

ChessItem::ChessItem(int userChessColor, QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , chessWhitePixmap(DHiDPIHelper::loadNxPixmap(":/resources/white_chess.svg"))
    , chessBlackPixmap(DHiDPIHelper::loadNxPixmap(":/resources/black_chess.svg"))
    , chessType(userChessColor)
{
    setAcceptHoverEvents(true);
}

//设置棋子
void ChessItem::setCurrentchess(int chesstype)
{
    chessType = chesstype;
}

//获取棋子颜色
int ChessItem::getChessColor()
{
    return chessType;
}

void ChessItem::setHasPrintChess()
{
    chessHasPrint = true;
    update();
}

//设置是否有棋子
void ChessItem::setchessStatus(bool chessstatus)
{
    chessStatus = chessstatus;
    update();
}

//获取是否有棋子
bool ChessItem::getchessStatus()
{
    return chessStatus;
}

//获取当前旗手
int ChessItem::getChessPlayer()
{
    return isAIPlaying;
}

/**
 * @brief ChessItem::hoverHandle
 */
void ChessItem::hoverHandle()
{
    if (!gameOver && gameStatus) {
        //当前位置没有棋子,并且非AI下棋设置棋子颜色
        if (!chessStatus && !getChessPlayer() && m_isCurrentItem) {
            hoverStatus = true;
            setCurrentchess(chessType);
        }
    }
}

void ChessItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QColor chessColor;
    QPixmap chessPixmap;
    QPixmap chessPixmapNow;
    painter->setRenderHints(QPainter::Antialiasing);

    if (chessStatus) {
        painter->save();
        painter->setPen(Qt::NoPen);
        if (chessType == chess_white) {
            chessPixmap = chessWhitePixmap;
            chessPixmapNow = DHiDPIHelper::loadNxPixmap(":/resources/chess_white_now.svg");

        } else if (chessType == chess_black) {
            chessPixmap = chessBlackPixmap;
            chessPixmapNow = DHiDPIHelper::loadNxPixmap(":/resources/chess_balck_now.svg");
        }
        if (chessHasPrint) {
            painter->drawPixmap(QPointF(boundingRect().x(), boundingRect().y()), chessPixmap);
        } else {
            painter->drawPixmap(QPointF(boundingRect().x(), boundingRect().y()), chessPixmapNow);
        }
        painter->restore();
    } else {
        if (hoverStatus) {
            painter->save();
            painter->setPen(Qt::NoPen);

            QColor backColor("000000");
            backColor.setAlphaF(0.2);
            painter->setBrush(backColor);

            //绘制hover外层的阴影效果
            int outerR = 12;
            painter->drawEllipse(QRect(-outerR, -outerR, outerR*2, outerR*2));
            painter->restore();

            //绘制白点或黑点
            painter->save();
            painter->setPen(Qt::NoPen);
            if (chessType == chess_white) {
                chessColor = Qt::white;
            } else if (chessType == chess_black) {
                chessColor = Qt::black;
                chessColor.setAlphaF(0.7);
            }
            painter->setBrush(chessColor);

            //绘制hover状态
            int innerR = 5;
            painter->drawEllipse(QRect(-innerR, -innerR, innerR*2, innerR*2));
            painter->restore();
        }
    }
}

QRectF ChessItem::boundingRect() const
{
    int chessR = 22;
    return QRectF(-chessR, -chessR, chessR*2, chessR*2);
}

void ChessItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    m_isCurrentItem = true;
    hoverHandle();
    QGraphicsItem::hoverEnterEvent(event);

}


void ChessItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    hoverStatus = false;
    m_isCurrentItem = false;
    QGraphicsItem::hoverLeaveEvent(event);
}

void ChessItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() & Qt::LeftButton) {
        QGraphicsItem::mouseReleaseEvent(event);
    }
}

void ChessItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() & Qt::LeftButton) {
        //玩家下棋才能落子
        if (contains(event->pos()) && hoverStatus) {
            setchessStatus(true);
            emit signalCPaintItem(this);
        }
    }
}

//游戏结束
void ChessItem::slotGameOver()
{
    gameOver = true;
}

//判断旗手
void ChessItem::slotIsAIPlaying(bool AIPlaying)
{
    isAIPlaying = AIPlaying;
    hoverHandle();
    update();
}

/**
 * @brief ChessItem::slotGameContinue 继续游戏
 */
void ChessItem::slotGameContinue()
{
    gameStatus = true;
}

//暂停游戏
void ChessItem::slotGameStop()
{
    gameStatus = false;
}

