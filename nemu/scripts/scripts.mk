.PHONY: count
count:
	@find . -type f -name "*.[ch]" -not -path "./tools/*" -exec cat {} + | grep -v "^$$" | wc -l | awk '{ printf "total number of lines: %d\n", $$0 }'
	@echo "edits made to the main branch:$(shell git diff master --shortstat)"
