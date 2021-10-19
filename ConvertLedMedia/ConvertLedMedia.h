#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ConvertLedMedia.h"

class ConvertLedMedia : public QMainWindow
{
	Q_OBJECT

public:
	ConvertLedMedia(QWidget* parent = Q_NULLPTR);

	void dropEvent(QDropEvent* event) override;
	void dragLeaveEvent(QDragLeaveEvent* event) override;
	void dragEnterEvent(QDragEnterEvent* event) override;

protected:
	bool m_fmtColor3{ false };
	bool m_saveOriginPath{ true };
	QString m_fileSuffix{ "lgaif" };
	
private:
	Ui::ConvertLedMediaClass ui{};
};
