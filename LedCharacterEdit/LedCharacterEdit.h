#pragma once

#include <QtDesigner/QtDesigner>
#include <QtWidgets/QWidget>

class QDESIGNER_WIDGET_EXPORT LedCharacterEdit : public QWidget
{
	Q_OBJECT

		Q_PROPERTY(int horizontalPixelCount READ horizontalPixelCount WRITE setHorizontalPixelCount DESIGNABLE false STORED false)
		Q_PROPERTY(int verticalPixelCount READ verticalPixelCount WRITE setVerticalPixelCount DESIGNABLE false STORED false)
		Q_PROPERTY(QSize pixelSize READ pixelSize RESET unsetColorMap WRITE setPixelSize  NOTIFY pixelSizeChanged USER true)
		Q_PROPERTY(QPixmap colorMap READ colorMap RESET unsetColorMap WRITE setColorMap NOTIFY colorMapChanged USER true)
		Q_PROPERTY(int pixelLength READ pixelLength WRITE setPixelLength)
		Q_PROPERTY(int outerBorderWidth READ outerBorderWidth WRITE setOuterBorderWidth)
		Q_PROPERTY(int innerBorderWidth READ innerBorderWidth WRITE setInnerBorderWidth)
		Q_PROPERTY(QColor outerBorderColor READ outerBorderColor WRITE setOuterBorderColor)
		Q_PROPERTY(QColor innerBorderColor READ innerBorderColor WRITE setInnerBorderColor)
		Q_PROPERTY(QColor pencil READ pencil WRITE setPencil NOTIFY pencilChanged)

public:
	LedCharacterEdit(QWidget* parent = Q_NULLPTR);

	int horizontalPixelCount() const;
	int verticalPixelCount() const;
	QSize pixelSize() const;

	QPixmap colorMap() const;

	int pixelLength() const;

	int outerBorderWidth() const;
	int innerBorderWidth() const;
	QColor outerBorderColor() const;
	QColor innerBorderColor() const;

	QColor pencil() const;
	
	QVector<QRgb> rgbEncode() const;
public slots:

	void setHorizontalPixelCount(int);
	void setVerticalPixelCount(int);
	void setPixelSize(QSize);

	void setColorMap(QPixmap);
	void unsetColorMap();

	void setPixelLength(int);

	void setOuterBorderWidth(int);
	void setInnerBorderWidth(int);
	void setOuterBorderColor(QColor);
	void setInnerBorderColor(QColor);

	void setPencil(QColor);

	void updateDetachedRenderPicture();
	void clearColorMap();
protected:
	void syncColorPixmap();
	void updateSize();
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void gridCellTriggered(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event) override;

	void doClearColorMap();

signals:
	void pixelDrawed(int, int, QColor) const;
	void pixelSizeChanged(QSize) const;
	void colorMapChanged(QPixmap) const;
	void pencilChanged(QColor) const;

	void privatePixelSizeChanged(QPrivateSignal) const;
	void privateColorMapChanged(QPrivateSignal) const;

protected:
	constexpr static QSize DefaultPixelSize{ 4, 8 };
	QSize m_pixelSize{ DefaultPixelSize };
	QImage m_colorImage{ DefaultPixelSize, QImage::Format_RGB32 };

	int m_pixelLength{ 16 };

	int m_outerBorderWidth{ 5 };
	int m_innerBorderWidth{ 1 };
	QColor m_outerBorderColor{ Qt::gray };
	QColor m_innerBorderColor{ Qt::gray };

	QColor m_pencil{ Qt::white };

private:
	QSharedPointer<QPicture> m_detachRenderPicture;
	mutable QPixmap m_colorPixmapCache{ DefaultPixelSize };
	mutable bool m_cacheUnable{ false };
	bool m_mouseDown{ false };
};
