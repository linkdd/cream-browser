LANG=en

messages.pot:
	@(cd ../src/ && xgettext --keyword=_ -c=// -o ../po/messages.pot *.c *.h)

create_po: messages.pot
	@msginit --locale=$(LANG) --input=$^

update_po: messages.pot
	@msgmerge -o $(LANG).po $(LANG).po $^

mo:
	@for po in *.po; do                \
		msgfmt -o $${po%.po}.mo $$po; \
	done

.PHONY: messages.pot create_po update_po mo
