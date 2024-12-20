SELECT 'Flash/Ignite' AS skills, SUM(win=1) / SUM(win=0) AS ratio
FROM (
    SELECT s.win AS win
    FROM stat AS s
    JOIN participant AS p ON s.player_id = p.player_id
    WHERE ((p.ss1 = 'FLASH' AND p.ss2 = 'IGNITE') OR (p.ss2 = 'FLASH' AND p.ss1 = 'IGNITE')) 
    AND p.position = 'TOP'
) AS temp
UNION
SELECT 'Flash/Teleport' as skills, SUM(win=1) / SUM(win=0) as ratio
FROM (
    SELECT s.win as win
    FROM stat AS s
    JOIN participant AS p ON s.player_id = p.player_id
    WHERE ((p.ss1 = 'FLASH' AND p.ss2 = 'TELEPORT') OR(p.ss2 = 'FLASH' AND p.ss1 = 'TELEPORT')) 
    AND p.position = 'TOP'
) AS temp;