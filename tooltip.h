#ifndef TOOLTIP_H
#define TOOLTIP_H

#include "knpanel_export.h"

class QPixmap;
class QString;
class QWidget;

namespace KNPanelToolTip
{
    KNPANEL_EXPORT void display( const QWidget* client, const QString& title, const QString& subtext, const QPixmap& icon );
    KNPANEL_EXPORT void hide();
}

#endif // TOOLTIP_H
