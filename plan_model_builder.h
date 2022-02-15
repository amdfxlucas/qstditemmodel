#ifndef PLAN_MODEL_BUILDER_H
#define PLAN_MODEL_BUILDER_H

#include "hplan_model.h"

class plan_model_builder
{private:
    QString _year;
public:
    plan_model_builder(QString year);

    hplan_model& build(hplan_model&);
};

#endif // PLAN_MODEL_BUILDER_H
