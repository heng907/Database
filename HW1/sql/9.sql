SELECT 
    SUBSTRING_INDEX(tmp.ver, '.', 2) AS version,
    SUM(tmp.win=1) AS win_cnt,
    SUM(tmp.win=0) AS lose_cnt, 
    SUM(tmp.win=1) / (SUM(tmp.win=0) + SUM(tmp.win=1)) AS win_ratio
FROM (
    SELECT s.win AS win, m.version AS ver
    FROM stat AS s 
    JOIN participant p ON s.player_id = p.player_id 
    JOIN match_info m ON p.match_id = m.match_id
    WHERE p.champion_id = 17 OR p.champion_id = 64
    GROUP BY s.win, p.match_id
    HAVING 
        FIND_IN_SET(17, GROUP_CONCAT(p.champion_id)) AND
        FIND_IN_SET(64, GROUP_CONCAT(p.champion_id))
) AS tmp
group by version;