TO_FORMAT = `find -name "**.h" -or -name "**.cpp"`

all: tags comp

comp:
	$(MAKE) -C can
	@echo --------------------------------
	$(MAKE) -C augreality
	@echo --------------------------------
	@echo Done!

format:
	clang-format -style=file -i	$(TO_FORMAT)

tags:
	@echo Generating tags...
	@echo --------------------------------
	@ctags -R --languages=c++

.PHONY: clean

clean:
	-rm tags
	@echo --------------------------------
	$(MAKE) clean -C can
	@echo --------------------------------
	$(MAKE) clean -C augreality
