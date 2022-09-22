if [ "$(tail -c 1 "$SCRIPT_ROOT/config/settings.ini")" != $'\n' ]; then
	echo >> $SCRIPT_ROOT/config/settings.ini
fi

while read -r line; do
	line=$(echo -n "$line" | sed -E 's/#.*$//g')
	if [ $(echo -n "$line" | tr -cd '=' | wc -c) -ne 1 ]; then
		echo "Config error: more than 1 '=' in a line"
		exit 1
	fi
	
	if [ -z "$(echo -n "$line" | tr -d '[:space:]')" ]; then
		continue
	fi
	
	field_name=$(echo -n "$line" | sed -E 's/\s*=\s*.+$//g')
	field_value=$(echo -n "$line" | sed -E 's/^.+\s*=\s*//g')
	
	declare -g "__config_$field_name=$field_value"
done < "$SCRIPT_ROOT/config/settings.ini"

function opt_or_default {
	if [ $# -ne 2 ]; then
		cat <<- {EOF}
		Usage: opt_or_default OPTION DEFAULT
		OPTION is the config option to use, DEFAULT is the value if it is not set
		{EOF}
		exit 1
	fi
	
	cfg_name="__config_$1"
	
	echo "${!cfg_name-$2}"
}