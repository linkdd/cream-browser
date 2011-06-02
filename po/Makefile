LANG=en

messages.pot:
	@xgettext -f POTFILES.in --keyword=_ -c=// -o messages.pot

create_po: messages.pot
	@msginit --locale=$(LANG) --input=$^

update_po: messages.pot
	@msgmerge -o $(LANG).po $(LANG).po $^

.PHONY: messages.pot create_po update_po
