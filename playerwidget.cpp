#include "playerwidget.h"

#include <QState>
#include <QFileDialog>
#include <QMimeData>
#include <QUrl>


PlayerWidget::PlayerWidget(QWidget *parent)
    : QWidget(parent)
{
    main_layout = new QVBoxLayout(this);
    video_widget = new VideoWidget(this);
    player = new QMediaPlayer(this);
    controller = new Controller(this);

    player->setVideoOutput(video_widget);
    player->setVolume(50);
    main_layout->setSpacing(0);
    main_layout->setMargin(0);

    main_layout->addWidget(video_widget);
    main_layout->addLayout(controller);
    setLayout(main_layout);

    video_widget->setAcceptDrops(true);//设置支持拖拽

    video_widget->show();


    connect(controller, &Controller::playChanged, this, &PlayerWidget::play);               // signal: playChanged() ,通过play中判断QmediaPlayer的暂停与否切换状态
    connect(controller, &Controller::volumeChanged, player, &QMediaPlayer::setVolume);      // signal: volumeChanged(qint64)
    connect(controller, &Controller::fullChanged, video_widget, &VideoWidget::fullScreen);  // signal: 通过判断QmWidget的暂停与否切换状态
    connect(player, &QMediaPlayer::stateChanged, controller, &Controller::stateChanged);    // signal: stateChanged(QtMediaPlayer::state ) |> 和 ||图标的转化
    connect(video_widget, &VideoWidget::playChanged, this, &PlayerWidget::play);            // 鼠标直接点击 videoWidget的暂停播放
    connect(video_widget, &VideoWidget::sendFile, this, &PlayerWidget::receiveFile);        //
    connect(video_widget, &VideoWidget::volumeChanged, this, &PlayerWidget::setVolume);     // 声音
    connect(video_widget, &VideoWidget::positionChanged, this, &PlayerWidget::setPosition); //  键盘上下左右 这俩都写在 各自的 keyrelease事件里了
    connect(controller, &Controller::positionChange, player, &QMediaPlayer::setPosition);   // 滑条的pos改变
    connect(player, &QMediaPlayer::positionChanged, controller, &Controller::setPosition);  // 自然播放控制滑条
    connect(player, &QMediaPlayer::durationChanged, controller, &Controller::setDuration);  // 总时长改变
    connect(player,&QMediaPlayer::volumeChanged, controller, &Controller::setVolume);       // 视频自然声音改变

    connect(controller, &Controller::comboBoxChange, this, [=](int idx){
        this->player->setPlaybackRate(idx+1);
    });
}

PlayerWidget::~PlayerWidget()
{
}

void PlayerWidget::openFile()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Video"), QDir::homePath(), \
                                                    tr("Video Files (*.mp4 *.flv *.mkv\
                                                                     *.avi *.rmvb *.3gp);;\
                                                        All Files (*.*)"));
    if(!filename.isEmpty()) {
        player->setMedia(QUrl::fromLocalFile(filename));
       // player->setMedia(QUrl("gst-pipeline: "+ filename + " ! autovideosink"));
        player->play();
    }
}

void PlayerWidget::play()
{
    switch(player->state()) {
    case QMediaPlayer::PlayingState:
        player->pause();
        break;
    case QMediaPlayer::PausedState:
        player->play();
        break;
    default:
        break;
    }
}

void PlayerWidget::receiveFile(QString filename)
{
    if(!filename.isEmpty()) {
        player->setMedia(QUrl(filename));
        player->play();
        controller->setDuration(player->duration());
    }
}

void PlayerWidget::keyReleaseEvent(QKeyEvent *event)
{
    switch(event->key()) {
    case Qt::Key_Space:
        play();
        break;
    case Qt::Key_Right:
        setPosition(qint64(15000));
        break;
    case Qt::Key_Left:
        setPosition(qint64(-10000));
        break;
    case Qt::Key_Up:
        setVolume(5);
        break;
    case Qt::Key_Down:
        setVolume(-5);
        break;
    default:
        break;
    }
}

void PlayerWidget::setVolume(int value)
{
    player->setVolume(player->volume() + value);
}

void PlayerWidget::setPosition(qint64 value)
{
    player->setPosition(player->position() + value);
}

