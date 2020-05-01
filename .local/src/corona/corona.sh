[ "$(stat -c %y ~/.local/src/corona/data 2>/dev/null | cut -d ' ' -f 1)" != "$(date '+%Y-%m-%d')" ] &&
    curl -s https://corona-stats.online?source=2 > ~/.local/src/corona/data

grep "Turkey" ~/.local/src/corona/data |
    sed -E "s/\x1b\[[0-9;]*m//g; s/\s*//g; s/║//g; s/│/;/g" | #First substitution removes color.
    awk -F ';' '{print $1") 😷"$3, "💀"$5, "💉"$7, $4}'
