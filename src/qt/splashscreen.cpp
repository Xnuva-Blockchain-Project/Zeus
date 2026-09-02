// Copyright (c) 2011-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#if defined(HAVE_CONFIG_H)
#include <config/bitcoin-config.h>
#endif

#include "splashscreen.h"

#include "networkstyle.h"

#include "clientversion.h"
#include "init.h"
#include "util.h"
#include "ui_interface.h"
#include "version.h"

#ifdef ENABLE_WALLET
#include "wallet/wallet.h"
#endif

#include <QApplication>
#include <QCloseEvent>
#include <QCursor>
#include <QDesktopWidget>
#include <QPainter>
#include <QRadialGradient>

SplashScreen::SplashScreen(Qt::WindowFlags f, const NetworkStyle *networkStyle) :
    QWidget(0, f), curAlignment(0)
{
    // set reference point, paddings
    int paddingTop              = 50;
    int titleVersionVSpace      = 17;
    int titleMaintenanceVSpace  = 34;
    int titleCopyrightVSpace    = 62;
    const int textLeft          = 285;

    float fontFactor            = 1.0;
    float devicePixelRatio      = 1.0;
#if QT_VERSION > 0x050100
    devicePixelRatio = ((QGuiApplication*)QCoreApplication::instance())->devicePixelRatio();
#endif

    // define text to place
    QString titleText       = tr("Zeus Wallet");
    QString versionText     = QString("Version %1").arg(QString::fromStdString(FormatFullVersion()));
    QString releaseText     = QString("2026 Maintained Release");
    QString maintenanceText = QString("2026 Maintained by the Xnuva Blockchain Project");

    QString copyrightTextBitcoin     = QChar(0xA9)+QString(" %1-%2 ").arg(2009).arg(COPYRIGHT_YEAR) + QString("The Bitcoin Core developers");
    QString copyrightTextBlackcoin   = QChar(0xA9)+QString(" %1-%2 ").arg(2014).arg(2018) + QString("The Blackcoin developers");
    QString copyrightTextBlackmore   = QChar(0xA9)+QString(" %1-%2 ").arg(2018).arg(COPYRIGHT_YEAR) + QString("The Blackcoin More developers");
    // QString copyrightText   = QChar(0xA9)+QString(" %1-%2 ").arg(2009).arg(COPYRIGHT_YEAR) + QString::fromStdString(CopyrightHolders());

    QString titleAddText    = networkStyle->getTitleAddText();

    QString font            = QApplication::font().toString();

    // create a bitmap according to device pixelratio
    QSize splashSize(480*devicePixelRatio,320*devicePixelRatio);
    pixmap = QPixmap(splashSize);

#if QT_VERSION > 0x050100
    // change to HiDPI if it makes sense
    pixmap.setDevicePixelRatio(devicePixelRatio);
#endif

    QPainter pixPaint(&pixmap);
    pixPaint.setPen(QColor(100,100,100));

    // draw a slightly radial gradient
    QRadialGradient gradient(QPoint(0,0), splashSize.width()/devicePixelRatio);
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(1, QColor(247,247,247));
    QRect rGradient(QPoint(0,0), splashSize);
    pixPaint.fillRect(rGradient, gradient);

    // draw the bitcoin icon, expected size of PNG: 1024x1024
    QRect rectIcon(QPoint(0,10), QSize(300,300));

    const QSize requiredSize(1024,1024);
    QPixmap icon(networkStyle->getAppIcon().pixmap(requiredSize));

    pixPaint.drawPixmap(rectIcon, icon);

    // check font size and drawing with
    pixPaint.setFont(QFont(font, 33*fontFactor));
    QFontMetrics fm = pixPaint.fontMetrics();
    int titleTextWidth = fm.width(titleText);
    if (titleTextWidth > 176) {
        fontFactor = fontFactor * 176 / titleTextWidth;
    }

    pixPaint.setFont(QFont(font, 33*fontFactor));
    fm = pixPaint.fontMetrics();
    titleTextWidth  = fm.width(titleText);
    pixPaint.drawText(textLeft,paddingTop,titleText);

    pixPaint.setFont(QFont(font, 15*fontFactor));

    // if the version string is to long, reduce size
    fm = pixPaint.fontMetrics();
    int versionTextWidth  = fm.width(versionText);
    if(versionTextWidth > 180) {
        pixPaint.setFont(QFont(font, 10*fontFactor));
        titleVersionVSpace -= 5;
    }
    pixPaint.drawText(textLeft,paddingTop+titleVersionVSpace,versionText);

    // Make the maintenance year immediately visible on startup.
    pixPaint.setFont(QFont(font, 10*fontFactor));
    pixPaint.drawText(textLeft,paddingTop+titleMaintenanceVSpace,releaseText);

    // Draw historical copyright notices unchanged, followed by
    // the current maintenance identity without claiming copyright.
    {
        pixPaint.setFont(QFont(font, 9*fontFactor));
        const int x = textLeft;
        const int y = paddingTop+titleCopyrightVSpace;

        pixPaint.drawText(x,y,copyrightTextBitcoin);
        pixPaint.drawText(x,y+10,copyrightTextBlackcoin);
        pixPaint.drawText(x,y+20,copyrightTextBlackmore);

        pixPaint.setFont(QFont(font, 8*fontFactor));
        pixPaint.drawText(
            QRect(x,y+27,180,32),
            Qt::AlignLeft | Qt::TextWordWrap,
            maintenanceText
        );
    }

    // draw additional text if special network
    if(!titleAddText.isEmpty()) {
        QFont boldFont = QFont(font, 10*fontFactor);
        boldFont.setWeight(QFont::Bold);
        pixPaint.setFont(boldFont);
        fm = pixPaint.fontMetrics();
        int titleAddTextWidth  = fm.width(titleAddText);
        pixPaint.drawText(pixmap.width()/devicePixelRatio-titleAddTextWidth-10,15,titleAddText);
    }

    pixPaint.end();

    // Set window title
    setWindowTitle(titleText + " " + titleAddText);

    // Resize window and move to center of desktop, disallow resizing
    QRect r(QPoint(), QSize(pixmap.size().width()/devicePixelRatio,pixmap.size().height()/devicePixelRatio));
    resize(r.size());
    setFixedSize(r.size());
    QDesktopWidget *desktop = QApplication::desktop();
    int screenNumber = desktop->screenNumber(QCursor::pos());

    if (screenNumber < 0)
        screenNumber = desktop->primaryScreen();

    const QRect screenGeometry = desktop->availableGeometry(screenNumber);
    move(screenGeometry.center() - r.center());

    subscribeToCoreSignals();
}

SplashScreen::~SplashScreen()
{
    unsubscribeFromCoreSignals();
}

void SplashScreen::slotFinish(QWidget *mainWin)
{
    Q_UNUSED(mainWin);

    /* If the window is minimized, hide() will be ignored. */
    /* Make sure we de-minimize the splashscreen window before hiding */
    if (isMinimized())
        showNormal();
    hide();
    deleteLater(); // No more need for this
}

static void InitMessage(SplashScreen *splash, const std::string &message)
{
    QMetaObject::invokeMethod(splash, "showMessage",
        Qt::QueuedConnection,
        Q_ARG(QString, QString::fromStdString(message)),
        Q_ARG(int, Qt::AlignBottom|Qt::AlignHCenter),
        Q_ARG(QColor, QColor(55,55,55)));
}

static void ShowProgress(SplashScreen *splash, const std::string &title, int nProgress)
{
    InitMessage(splash, title + strprintf("%d", nProgress) + "%");
}

#ifdef ENABLE_WALLET
static void ConnectWallet(SplashScreen *splash, CWallet* wallet)
{
    wallet->ShowProgress.connect(boost::bind(ShowProgress, splash, _1, _2));
}
#endif

void SplashScreen::subscribeToCoreSignals()
{
    // Connect signals to client
    uiInterface.InitMessage.connect(boost::bind(InitMessage, this, _1));
    uiInterface.ShowProgress.connect(boost::bind(ShowProgress, this, _1, _2));
#ifdef ENABLE_WALLET
    uiInterface.LoadWallet.connect(boost::bind(ConnectWallet, this, _1));
#endif
}

void SplashScreen::unsubscribeFromCoreSignals()
{
    // Disconnect signals from client
    uiInterface.InitMessage.disconnect(boost::bind(InitMessage, this, _1));
    uiInterface.ShowProgress.disconnect(boost::bind(ShowProgress, this, _1, _2));
#ifdef ENABLE_WALLET
    if(pwalletMain)
        pwalletMain->ShowProgress.disconnect(boost::bind(ShowProgress, this, _1, _2));
#endif
}

void SplashScreen::showMessage(const QString &message, int alignment, const QColor &color)
{
    curMessage = message;
    curAlignment = alignment;
    curColor = color;
    update();
}

void SplashScreen::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, pixmap);
    QRect r = rect().adjusted(5, 5, -5, -5);
    painter.setPen(curColor);
    painter.drawText(r, curAlignment, curMessage);
}

void SplashScreen::closeEvent(QCloseEvent *event)
{
    StartShutdown(); // allows an "emergency" shutdown during startup
    event->ignore();
}
