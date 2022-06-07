// PWH.
#pragma once

#include <dime/Input.h>
#include <dime/Output.h>
#include <dime/Model.h>
#include <dime/RecordHolder.h>

#include <dime/convert/convert.h>
#include <dime/convert/layerdata.h>

#include <dime/entities/3DFace.h>
#include <dime/entities/Arc.h>
#include <dime/entities/Block.h>
#include <dime/entities/Circle.h>
#include <dime/entities/Ellipse.h>
#include <dime/entities/ExtrusionEntity.h>
#include <dime/entities/FaceEntity.h>
#include <dime/entities/Insert.h>
#include <dime/entities/Line.h>
#include <dime/entities/LWPolyline.h>
#include <dime/entities/Point.h>
#include <dime/entities/Polyline.h>
#include <dime/entities/Solid.h>
#include <dime/entities/Spline.h>
#include <dime/entities/Text.h>
#include <dime/entities/Trace.h>
#include <dime/entities/UnknownEntity.h>
#include <dime/entities/Vertex.h>

#include <dime/sections/BlocksSection.h>
#include <dime/sections/ClassesSection.h>
#include <dime/sections/EntitiesSection.h>
#include <dime/sections/HeaderSection.h>
#include <dime/sections/ObjectsSection.h>
#include <dime/sections/TablesSection.h>
#include <dime/sections/UnknownSection.h>

#include <dime/tables/LayerTable.h>
#include <dime/tables/Table.h>
#include <dime/tables/UCSTable.h>
#include <dime/tables/UnknownTable.h>
