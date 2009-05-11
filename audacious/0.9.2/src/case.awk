BEGIN {
	FS = ","
	print "\
#ifdef ENABLE_CASE_C"
}

{
	if (NF == 3) {
		# Trim
		for (i = 1; i <= NF; i++) {
			sub("^ +", "", $i);
			sub(" +$", "", $i);
		}

		sub("^", "case ", $1);
		gsub("\\|", ": /* fall through */\n\tcase ", $1);
		sub("$", ":", $1);

		CASE_LABEL=$1
		DATA_TYPE=$2
		EFFECT_FUNCTION=$3

		# Print filled template
		print "\
	" CASE_LABEL "\n\
		{\n\
			const gint num = length / sizeof(" DATA_TYPE ") / 2;\n\
			" DATA_TYPE " * const sample = (" DATA_TYPE " *)*data;\n\
			" EFFECT_FUNCTION "(bs2b, sample, num);\n\
		}\n\
		break;"
	}
}

END {
	print "\
#endif /* ENABLE_CASE_C */"
}