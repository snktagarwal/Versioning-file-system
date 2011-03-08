#include "label.h"

Label::Label(const QString &text) : QString(text) {
}

bool Label::sceneEventFilter(Point *p, QEvent *event) {
	setPlainText(p->getX());
	return false;
}
