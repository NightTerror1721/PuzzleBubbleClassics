import "base"
funcs = import "funcs"

name = "ColorBubble";

model = Bubbles.BubbleModel.create(name)

model.colorless = false
model.multicolor = false

model.floating = false
model.destroyInBottom = false
model.requireDestroyToClear = true
model.onlyBoardColorInArrowGen = true
print("onlyBoardColorInArrowGen")

model.resistence = 0

print("resistence")
model.pointsOfTurnsToDown = 1

print("here!")
model.onInit = function(self, color, editorMode)
    print("ola")
end


--print(funcs.plus(10, 25))
print("funcs")
