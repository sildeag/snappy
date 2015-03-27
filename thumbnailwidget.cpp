#include "thumbnailwidget.h"

ThumbnailWidget::ThumbnailWidget(QString photoPath, QWidget *parent) : QWidget(parent)
{
    this->photoPath = photoPath;
    layout = new QGridLayout();
    label = new QLabel(this);
    layout->addWidget(label);
    setImage();
    label->setAlignment(Qt::AlignCenter);
    label->setMinimumSize(80,80);
    setLayout(layout);
}

ThumbnailWidget::~ThumbnailWidget()
{
    delete layout;
    delete label;
}


void ThumbnailWidget::setImage() {
    QMatrix rotation = QMatrix();
    ExifData* exifData = exif_data_new_from_file(photoPath.toStdString().c_str());
    ExifContent* content = *exifData->ifd;
    ExifEntry* entry = exif_content_get_entry(content,EXIF_TAG_ORIENTATION);
    if(entry!=NULL) {
        std::string orientation = std::string((const char*)entry->data);
        if(orientation=="Rotated 90 CCW") {
            rotation.rotate(90);
        } else if(orientation=="Rotated 90 CW") {
            rotation.rotate(-90);
        } else if(orientation=="Rotated 180") {
            rotation.rotate(180);
        }
    }
    if(!ifThumbExists()) {
        Importer::createThumbnail(photoPath);
    }
    QImage icon(getThumbPath());
    icon = icon.transformed(rotation);
    QSize size(80,80);
    label->setPixmap(QPixmap::fromImage(icon).scaled(size,Qt::KeepAspectRatio));
}

QString ThumbnailWidget::getThumbPath() {
    QFileInfo mainPhotoInfo(photoPath);
    return mainPhotoInfo.dir().absolutePath().append("/.thumbnails/").append(mainPhotoInfo.fileName());
}

bool ThumbnailWidget::ifThumbExists() {
    QFileInfo thumb(getThumbPath());
    if(thumb.exists() && thumb.isFile()) {
        return true;
    }
    return false;
}