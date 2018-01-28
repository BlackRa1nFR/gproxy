hook.Remove("HUDPaint", "test_esp", function()
    for k,v in pairs(player.GetAll()) do 
        if (v:IsDormant()) then
            continue
        end
        local pos = v:GetPos():ToScreen()
        if (not pos.visible) then
            continue
        end
        draw.SimpleTextOutlined(debug.getregistry().Player.Nick(v), "DermaDefault", pos.x, pos.y, nil, TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER, 1, Color(0,0,0,255))
    end
end)