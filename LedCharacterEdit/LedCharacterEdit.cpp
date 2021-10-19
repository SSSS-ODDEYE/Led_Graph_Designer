#include "LedCharacterEdit.h"

#include <QPainter>
#include <QPicture>

LedCharacterEdit::LedCharacterEdit(QWidget* parent)
	: QWidget(parent)
{
	m_detachRenderPicture = QSharedPointer<QPicture>::create();

	setCursor(Qt::CrossCursor);
	clearColorMap();
	updateSize();

	connect(this, &LedCharacterEdit::privatePixelSizeChanged, [this]
		{
			doClearColorMap();
			updateSize();
			emit pixelSizeChanged(pixelSize());
			emit colorMapChanged(colorMap());
		});
	connect(this, &LedCharacterEdit::privateColorMapChanged, [this]
		{
			if (m_pixelSize != colorMap().size())
			{
				m_pixelSize = colorMap().size();
				emit pixelSizeChanged(m_pixelSize);
			}
			emit colorMapChanged(colorMap());
			updateSize();
		});
}

int LedCharacterEdit::horizontalPixelCount() const
{
	return m_pixelSize.width();
}

int LedCharacterEdit::verticalPixelCount() const
{
	return m_pixelSize.height();
}

QSize LedCharacterEdit::pixelSize() const
{
	return m_pixelSize;
}

QPixmap LedCharacterEdit::colorMap() const
{
	if (m_cacheUnable)
	{
		m_cacheUnable = false;
		m_colorPixmapCache = QPixmap::fromImage(m_colorImage);
	}
	return m_colorPixmapCache;
}

void LedCharacterEdit::setHorizontalPixelCount(int w)
{
	if (w == horizontalPixelCount())
		return;
	m_pixelSize.setWidth(w);
	emit privatePixelSizeChanged(QPrivateSignal());
}

void LedCharacterEdit::setVerticalPixelCount(int h)
{
	if (h == verticalPixelCount())
		return;
	m_pixelSize.setHeight(h);
	emit privatePixelSizeChanged(QPrivateSignal());
}

void LedCharacterEdit::setPixelSize(QSize s)
{
	if (s == pixelSize())
		return;
	m_pixelSize = s;
	emit privatePixelSizeChanged(QPrivateSignal());
}

void LedCharacterEdit::unsetColorMap()
{
	m_pixelSize = DefaultPixelSize;
	emit privatePixelSizeChanged(QPrivateSignal());
}

void LedCharacterEdit::setColorMap(QPixmap m)
{
	if (m.isNull())
		return;
	m_colorImage = m.toImage();
	emit privateColorMapChanged(QPrivateSignal());
}

int LedCharacterEdit::pixelLength() const
{
	return m_pixelLength;
}

void LedCharacterEdit::setPixelLength(int l)
{
	m_pixelLength = l;
	updateSize();
}

int LedCharacterEdit::outerBorderWidth() const
{
	return m_outerBorderWidth;
}

int LedCharacterEdit::innerBorderWidth() const
{
	return m_innerBorderWidth;
}

QColor LedCharacterEdit::outerBorderColor() const
{
	return m_outerBorderColor;
}

QColor LedCharacterEdit::innerBorderColor() const
{
	return m_innerBorderColor;
}

QColor LedCharacterEdit::pencil() const
{
	return m_pencil;
}

QVector<QRgb> LedCharacterEdit::rgbEncode() const
{
	const auto hCount{ horizontalPixelCount() };
	const auto vCount{ verticalPixelCount() };
	static QVector<QRgb> buff;
	if (buff.size() != hCount * vCount)
		buff.resize(hCount * vCount);
	
	const auto bytesPerPixel{ m_colorImage.depth() / 8 };

	for (int j = 0; j < vCount; ++j)
	{
		auto lineDataPtr{ m_colorImage.constScanLine(j) };

		for (int i = 0; i < hCount; ++i)
		{
			buff[j * hCount + i] = qRgb(lineDataPtr[2], lineDataPtr[1], lineDataPtr[0]);
			lineDataPtr += bytesPerPixel;
		}
	}
	return buff;
}

void LedCharacterEdit::setOuterBorderWidth(int w)
{
	if (w < 0 || w == outerBorderWidth())
		return;
	m_outerBorderWidth = w;
	updateSize();
}

void LedCharacterEdit::setInnerBorderWidth(int w)
{
	if (w < 0 || w == innerBorderWidth())
		return;
	m_innerBorderWidth = w;
	updateSize();
}

void LedCharacterEdit::setOuterBorderColor(QColor c)
{
	if (c == outerBorderColor())
		return;
	m_outerBorderColor = c;
	updateDetachedRenderPicture();
}

void LedCharacterEdit::setInnerBorderColor(QColor c)
{
	if (c == innerBorderColor())
		return;
	m_innerBorderColor = c;
	updateDetachedRenderPicture();
}

void LedCharacterEdit::setPencil(QColor c)
{
	if (c == pencil())
		return;
	m_pencil = c;
	emit pencilChanged(m_pencil);
}

void LedCharacterEdit::updateDetachedRenderPicture()
{
	QPainter painter;
	painter.begin(m_detachRenderPicture.data());

	const auto hCount{ horizontalPixelCount() };
	const auto vCount{ verticalPixelCount() };

	if (innerBorderWidth() > 0)
	{
		for (int i = 1; i < hCount; ++i)
		{
			painter.fillRect(i * (pixelLength() + innerBorderWidth()) - innerBorderWidth() + outerBorderWidth(), 0,
				innerBorderWidth(), height(), innerBorderColor());
		}

		for (int i = 1; i < vCount; ++i)
		{
			painter.fillRect(0, i * (pixelLength() + innerBorderWidth()) - innerBorderWidth() + outerBorderWidth(),
				width(), innerBorderWidth(), innerBorderColor());
		}
	}


	const auto bytesPerPixel{ m_colorImage.depth() / 8 };

	for (int j = 0; j < vCount; ++j)
	{
		auto lineDataPtr{ m_colorImage.constScanLine(j) };

		for (int i = 0; i < hCount; ++i)
		{
			painter.fillRect(i * (pixelLength() + innerBorderWidth()) + outerBorderWidth(), j * (pixelLength() + innerBorderWidth()) + outerBorderWidth(),
				pixelLength(), pixelLength(),
				QColor(lineDataPtr[2], lineDataPtr[1], lineDataPtr[0])
			);
			lineDataPtr += bytesPerPixel;
		}
	}

	for (int i = 0; i < hCount; ++i)
	{
		for (int j = 0; j < vCount; ++j)
		{
			painter.fillRect(i * (pixelLength() + innerBorderWidth()) + outerBorderWidth(), j * (pixelLength() + innerBorderWidth()) + outerBorderWidth(),
				pixelLength(), pixelLength(),
				m_colorImage.pixelColor(i, j)
			);
		}
	}

	if (outerBorderWidth() > 0)
	{
		painter.fillRect(0, 0, width(), outerBorderWidth(), outerBorderColor());
		painter.fillRect(0, 0, outerBorderWidth(), height(), outerBorderColor());
		painter.fillRect(width() - outerBorderWidth() - 1, 0, width(), height(), outerBorderColor());
		painter.fillRect(0, height() - outerBorderWidth() - 1, width(), height(), outerBorderColor());
	}
	painter.end();
	update();
}

void LedCharacterEdit::clearColorMap()
{
	doClearColorMap();
	emit colorMapChanged(colorMap());
}

void LedCharacterEdit::syncColorPixmap()
{
	m_cacheUnable = true;
	updateDetachedRenderPicture();
}

void LedCharacterEdit::updateSize()
{
	setFixedSize(horizontalPixelCount() * (pixelLength() + innerBorderWidth()) - innerBorderWidth() + 2 * outerBorderWidth(),
		verticalPixelCount() * (pixelLength() + innerBorderWidth()) - innerBorderWidth() + 2 * outerBorderWidth());

	updateDetachedRenderPicture();
}

void LedCharacterEdit::paintEvent(QPaintEvent* event)
{
	QPainter{ this }.drawPicture(0, 0, *m_detachRenderPicture);
}

void LedCharacterEdit::mousePressEvent(QMouseEvent* event)
{
	m_mouseDown = true;
	if (m_mouseDown)
	{
		gridCellTriggered(event);
	}
	QWidget::mousePressEvent(event);
}

void LedCharacterEdit::mouseReleaseEvent(QMouseEvent* event)
{
	m_mouseDown = false;
	QWidget::mouseReleaseEvent(event);
}

void LedCharacterEdit::gridCellTriggered(QMouseEvent* event)
{
	auto pos = event->pos();
	pos -= {outerBorderWidth(), outerBorderWidth()};
	if (pos.x() % (pixelLength() + innerBorderWidth()) < pixelLength()
		&& pos.y() % (pixelLength() + innerBorderWidth()))
	{
		pos.setX(pos.x() / (innerBorderWidth() + pixelLength()));
		pos.setY(pos.y() / (innerBorderWidth() + pixelLength()));

		if (m_colorImage.pixelColor(pos) != pencil())
		{
			m_colorImage.setPixelColor(pos, pencil());
			syncColorPixmap();
			emit pixelDrawed(pos.x(), pos.y(), pencil());
		}
	}
}

void LedCharacterEdit::mouseMoveEvent(QMouseEvent* event)
{
	if (m_mouseDown)
	{
		gridCellTriggered(event);
	}
	QWidget::mouseMoveEvent(event);
}

void LedCharacterEdit::doClearColorMap()
{
	m_colorImage = QImage(pixelSize(), QImage::Format_RGB32);
	m_colorImage.fill(Qt::black);
	syncColorPixmap();
}
