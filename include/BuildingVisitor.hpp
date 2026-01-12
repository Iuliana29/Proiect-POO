#ifndef OOP_BUILDINGVISITOR_HPP
#define OOP_BUILDINGVISITOR_HPP

class ResidentialBuilding;
class UtilityBuilding;
class Park;
class CommercialBuilding;
class FactoryBuilding;

class BuildingVisitor {
public:
    virtual ~BuildingVisitor() = default;

    virtual void visit(ResidentialBuilding&) = 0;
    virtual void visit(UtilityBuilding&) = 0;
    virtual void visit(Park&) = 0;
    virtual void visit(CommercialBuilding&) = 0;
    virtual void visit(FactoryBuilding&) = 0;
};

#endif
