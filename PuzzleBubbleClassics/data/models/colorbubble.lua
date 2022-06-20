openlib "PBC.bubbles"
openlib "base"

local name <const> = "ColorBubble";
local model <const> = PBC.bubbles.BubbleModel.create(name)

model.colorless = false
model.multicolor = false

model.floating = false
model.destroyInBottom = false
model.requireDestroyToClear = true
model.onlyBoardColorInArrowGen = true

model.resistence = 0

model.pointsOfTurnsToDown = 1

model.onInit = function(self, color, editorMode)
    print("ola")
end
