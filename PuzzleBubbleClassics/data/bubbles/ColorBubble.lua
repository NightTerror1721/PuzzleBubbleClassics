

local Floating <const> = false
local DestroyInBottom <const> = false
local RequireDestroyToClear <const> = true
local OnlyBoardColorInArrowGen <const> = true

local Resistance <const> = 0

local PointsOfTurnsToDown <const> = 1


function OnInit()
    print("ola k ase "..PointsOfTurnsToDown)
end


function OnConstruct(bubble, color, editorMode)
end


function OnCollide(projectile, target)
end


function OnInserted(bubble)
end


function OnExplode(bubble)
end


function OnNeighborInserted(bubble, neighbor)
end


function OnNeighborExplode(bubble, neighbor)
end
