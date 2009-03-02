BEGIN {
	FS = ","
	print "\
#ifdef ENABLE_CASE_C"
}

{
	if (NF == 5) {
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
		DATA_ENDIAN=$3
		SWAP_FUNCTION=$4
		EFFECT_FUNCTION=$5

		# Print filled template
		print "\
	" CASE_LABEL "\n\
		{\n\
			gint num = length / sizeof(" DATA_TYPE ") / 2;\n\
			" DATA_TYPE " * sample = (" DATA_TYPE " *)*data;\n\
			while (num--) {"

		if (DATA_ENDIAN != "AB_SYSTEM_ENDIAN") {
			print "\
				#if " DATA_ENDIAN " != AB_SYSTEM_ENDIAN\n\
					" SWAP_FUNCTION "(sample);\n\
				#endif"
		}

		print "\
				" EFFECT_FUNCTION "(bs2b, sample);"

		if (DATA_ENDIAN != "AB_SYSTEM_ENDIAN") {
			print "\
				#if " DATA_ENDIAN " != AB_SYSTEM_ENDIAN\n\
					" SWAP_FUNCTION "(sample);\n\
				#endif"
		}

		print "\
				sample += 2;\n\
			}\n\
		}\n\
		break;"
	}
}

END {
	print "\
#endif /* ENABLE_CASE_C */"
}