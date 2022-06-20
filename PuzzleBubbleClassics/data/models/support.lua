openlib "PBC.utils"
openlib "base"

local supportScriptName <const> = "support.lua"
local configScriptName <const> = "config.lua"

function getCurrentScriptName() return PBC.utils.files.filename(utils.getCurrentScriptPath()) end

function runDirectoryScripts()
    local currentDir = PBC.utils.getCurrentScriptDirectory()
    local currentScriptName = getCurrentScriptName()
    PBC.utils.files.forEachEntryInDirectory(currentDir, function(entry)
        if PBC.utils.files.isFile(entry) then
            local entryName = PBC.utils.files.filename(entry)
            if entryName ~= configScriptName and entryName ~= supportScriptName and entryName ~= currentScriptName then
                import(entry)
            end
        end
    end)
end
