-- MySQL Workbench Forward Engineering

SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION';

-- -----------------------------------------------------
-- Schema ASL_db
-- -----------------------------------------------------

-- -----------------------------------------------------
-- Schema ASL_db
-- -----------------------------------------------------
CREATE SCHEMA IF NOT EXISTS `ASL_db` DEFAULT CHARACTER SET utf8 ;
USE `ASL_db` ;

-- -----------------------------------------------------
-- Table `ASL_db`.`Paziente`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `ASL_db`.`Paziente` ;

CREATE TABLE IF NOT EXISTS `ASL_db`.`Paziente` (
  `tessera_sanitaria` VARCHAR(45) NOT NULL,
  `nome` VARCHAR(45) NOT NULL,
  `cognome` VARCHAR(45) NOT NULL,
  `indirizzo` VARCHAR(100) NOT NULL,
  `dataN` DATE NOT NULL,
  `luogoN` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`tessera_sanitaria`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `ASL_db`.`Telefono`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `ASL_db`.`Telefono` ;

CREATE TABLE IF NOT EXISTS `ASL_db`.`Telefono` (
  `numero` VARCHAR(45) NOT NULL,
  `tipo` ENUM('cellulare', 'fisso') NOT NULL,
  `paziente` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`numero`),
  INDEX `fk_Telefono_Paziente_idx` (`paziente` ASC) VISIBLE,
  CONSTRAINT `fk_Telefono_Paziente`
    FOREIGN KEY (`paziente`)
    REFERENCES `ASL_db`.`Paziente` (`tessera_sanitaria`)
    ON DELETE CASCADE
    ON UPDATE CASCADE)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `ASL_db`.`Email`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `ASL_db`.`Email` ;

CREATE TABLE IF NOT EXISTS `ASL_db`.`Email` (
  `posta` VARCHAR(45) NOT NULL,
  `paziente` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`posta`),
  INDEX `fk_Email_Paziente1_idx` (`paziente` ASC) VISIBLE,
  CONSTRAINT `fk_Email_Paziente1`
    FOREIGN KEY (`paziente`)
    REFERENCES `ASL_db`.`Paziente` (`tessera_sanitaria`)
    ON DELETE CASCADE
    ON UPDATE CASCADE)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `ASL_db`.`Esame`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `ASL_db`.`Esame` ;

CREATE TABLE IF NOT EXISTS `ASL_db`.`Esame` (
  `examID` INT NOT NULL AUTO_INCREMENT,
  `descrizione` VARCHAR(100) NOT NULL,
  `costo` FLOAT(8) NOT NULL,
  PRIMARY KEY (`examID`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `ASL_db`.`Parametro`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `ASL_db`.`Parametro` ;

CREATE TABLE IF NOT EXISTS `ASL_db`.`Parametro` (
  `nome` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`nome`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `ASL_db`.`Personale_anagrafica`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `ASL_db`.`Personale_anagrafica` ;

CREATE TABLE IF NOT EXISTS `ASL_db`.`Personale_anagrafica` (
  `CF` VARCHAR(45) NOT NULL,
  `nome` VARCHAR(45) NOT NULL,
  `cognome` VARCHAR(45) NOT NULL,
  `indirizzo` VARCHAR(100) NOT NULL,
  `tipo` ENUM('medico', 'primario', 'volontario') NOT NULL,
  `associazione` VARCHAR(45) NULL DEFAULT NULL,
  PRIMARY KEY (`CF`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `ASL_db`.`Ospedale`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `ASL_db`.`Ospedale` ;

CREATE TABLE IF NOT EXISTS `ASL_db`.`Ospedale` (
  `hospID` INT NOT NULL AUTO_INCREMENT,
  `nome` VARCHAR(45) NOT NULL,
  `indirizzo` VARCHAR(100) NOT NULL,
  `primario` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`hospID`),
  INDEX `fk_Ospedale_Personale1_idx` (`primario` ASC) VISIBLE,
  CONSTRAINT `fk_Ospedale_Personale1`
    FOREIGN KEY (`primario`)
    REFERENCES `ASL_db`.`Personale_anagrafica` (`CF`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `ASL_db`.`Laboratorio`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `ASL_db`.`Laboratorio` ;

CREATE TABLE IF NOT EXISTS `ASL_db`.`Laboratorio` (
  `hospID` INT NOT NULL,
  `labID` INT NOT NULL,
  `nome` VARCHAR(45) NOT NULL,
  `piano` INT NOT NULL,
  `stanza` VARCHAR(45) NOT NULL,
  `primario` VARCHAR(45) NOT NULL,
  INDEX `fk_Laboratorio_Ospedale1_idx` (`hospID` ASC) VISIBLE,
  INDEX `fk_Laboratorio_Personale1_idx` (`primario` ASC) VISIBLE,
  PRIMARY KEY (`hospID`, `labID`),
  CONSTRAINT `fk_Laboratorio_Ospedale1`
    FOREIGN KEY (`hospID`)
    REFERENCES `ASL_db`.`Ospedale` (`hospID`)
    ON DELETE CASCADE
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_Laboratorio_Personale1`
    FOREIGN KEY (`primario`)
    REFERENCES `ASL_db`.`Personale_anagrafica` (`CF`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `ASL_db`.`Esame_effettivo`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `ASL_db`.`Esame_effettivo` ;

CREATE TABLE IF NOT EXISTS `ASL_db`.`Esame_effettivo` (
  `examID` INT NOT NULL,
  `paziente` VARCHAR(45) NOT NULL,
  `data` DATE NOT NULL,
  `ora` TIME NOT NULL,
  `urgenza` ENUM('si', 'no') NOT NULL,
  `diagnosi` VARCHAR(256) NULL DEFAULT NULL,
  `codiceP` INT NOT NULL,
  `laboratorio` INT NOT NULL,
  `ospedale` INT NOT NULL,
  INDEX `fk_Esame_effettivo_Esame1_idx` (`examID` ASC) VISIBLE,
  INDEX `fk_Esame_effettivo_Paziente1_idx` (`paziente` ASC) INVISIBLE,
  PRIMARY KEY (`examID`, `paziente`, `data`),
  INDEX `fk_Esame_effettivo_Laboratorio1_idx` (`ospedale` ASC, `laboratorio` ASC) VISIBLE,
  CONSTRAINT `fk_Esame_effettivo_Esame1`
    FOREIGN KEY (`examID`)
    REFERENCES `ASL_db`.`Esame` (`examID`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_Esame_effettivo_Paziente1`
    FOREIGN KEY (`paziente`)
    REFERENCES `ASL_db`.`Paziente` (`tessera_sanitaria`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_Esame_effettivo_Laboratorio1`
    FOREIGN KEY (`ospedale` , `laboratorio`)
    REFERENCES `ASL_db`.`Laboratorio` (`hospID` , `labID`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `ASL_db`.`Composizione`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `ASL_db`.`Composizione` ;

CREATE TABLE IF NOT EXISTS `ASL_db`.`Composizione` (
  `examID` INT NOT NULL,
  `parametro` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`examID`, `parametro`),
  INDEX `fk_Parametro_has_Esame_Esame1_idx` (`examID` ASC) INVISIBLE,
  INDEX `fk_Parametro_has_Esame_Parametro1_idx` (`parametro` ASC) VISIBLE,
  CONSTRAINT `fk_Parametro_has_Esame_Parametro1`
    FOREIGN KEY (`parametro`)
    REFERENCES `ASL_db`.`Parametro` (`nome`)
    ON DELETE NO ACTION
    ON UPDATE CASCADE,
  CONSTRAINT `fk_Parametro_has_Esame_Esame1`
    FOREIGN KEY (`examID`)
    REFERENCES `ASL_db`.`Esame` (`examID`)
    ON DELETE CASCADE
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `ASL_db`.`Reparto`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `ASL_db`.`Reparto` ;

CREATE TABLE IF NOT EXISTS `ASL_db`.`Reparto` (
  `hospID` INT NOT NULL,
  `repID` INT NOT NULL,
  `nome` VARCHAR(45) NOT NULL,
  `tel` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`hospID`, `repID`),
  INDEX `fk_Reparto_Ospedale1_idx` (`hospID` ASC) VISIBLE,
  CONSTRAINT `fk_Reparto_Ospedale1`
    FOREIGN KEY (`hospID`)
    REFERENCES `ASL_db`.`Ospedale` (`hospID`)
    ON DELETE CASCADE
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `ASL_db`.`Svolgimento`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `ASL_db`.`Svolgimento` ;

CREATE TABLE IF NOT EXISTS `ASL_db`.`Svolgimento` (
  `esame` INT NOT NULL,
  `paziente` VARCHAR(45) NOT NULL,
  `data` DATE NOT NULL,
  `personale` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`esame`, `paziente`, `data`),
  INDEX `fk_Svolgimento_Personale_anagrafica1_idx` (`personale` ASC) VISIBLE,
  CONSTRAINT `fk_Svolgimento_Esame_effettivo1`
    FOREIGN KEY (`esame` , `paziente` , `data`)
    REFERENCES `ASL_db`.`Esame_effettivo` (`examID` , `paziente` , `data`)
    ON DELETE CASCADE
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_Svolgimento_Personale_anagrafica1`
    FOREIGN KEY (`personale`)
    REFERENCES `ASL_db`.`Personale_anagrafica` (`CF`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `ASL_db`.`Specializzazione`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `ASL_db`.`Specializzazione` ;

CREATE TABLE IF NOT EXISTS `ASL_db`.`Specializzazione` (
  `titolo` VARCHAR(100) NOT NULL,
  PRIMARY KEY (`titolo`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `ASL_db`.`Specializzato`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `ASL_db`.`Specializzato` ;

CREATE TABLE IF NOT EXISTS `ASL_db`.`Specializzato` (
  `primario` VARCHAR(45) NOT NULL,
  `specializzazione` VARCHAR(100) NOT NULL,
  PRIMARY KEY (`primario`, `specializzazione`),
  INDEX `fk_Specializzazione_has_Personale_Personale1_idx` (`primario` ASC) VISIBLE,
  INDEX `fk_Specializzazione_has_Personale_Specializzazione1_idx` (`specializzazione` ASC) VISIBLE,
  CONSTRAINT `fk_Specializzazione_has_Personale_Specializzazione1`
    FOREIGN KEY (`specializzazione`)
    REFERENCES `ASL_db`.`Specializzazione` (`titolo`)
    ON DELETE CASCADE
    ON UPDATE CASCADE,
  CONSTRAINT `fk_Specializzazione_has_Personale_Personale1`
    FOREIGN KEY (`primario`)
    REFERENCES `ASL_db`.`Personale_anagrafica` (`CF`)
    ON DELETE CASCADE
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `ASL_db`.`Users`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `ASL_db`.`Users` ;

CREATE TABLE IF NOT EXISTS `ASL_db`.`Users` (
  `username` VARCHAR(45) NOT NULL,
  `password` CHAR(32) NOT NULL,
  `ruolo` ENUM('personaleCUP', 'amministratore') NOT NULL,
  PRIMARY KEY (`username`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `ASL_db`.`Personale_dati_lavorativi`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `ASL_db`.`Personale_dati_lavorativi` ;

CREATE TABLE IF NOT EXISTS `ASL_db`.`Personale_dati_lavorativi` (
  `personale` VARCHAR(45) NOT NULL,
  `ospedale` INT NOT NULL,
  `reparto` INT NOT NULL,
  PRIMARY KEY (`personale`),
  INDEX `fk_Personale_dati_lavorativi_Reparto1_idx` (`ospedale` ASC, `reparto` ASC) VISIBLE,
  CONSTRAINT `fk_Personale_dati_lavorativi_Personale_anagrafica1`
    FOREIGN KEY (`personale`)
    REFERENCES `ASL_db`.`Personale_anagrafica` (`CF`)
    ON DELETE CASCADE
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_Personale_dati_lavorativi_Reparto1`
    FOREIGN KEY (`ospedale` , `reparto`)
    REFERENCES `ASL_db`.`Reparto` (`hospID` , `repID`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `ASL_db`.`Risultato`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `ASL_db`.`Risultato` ;

CREATE TABLE IF NOT EXISTS `ASL_db`.`Risultato` (
  `esame` INT NOT NULL,
  `paziente` VARCHAR(45) NOT NULL,
  `data` DATE NOT NULL,
  `parametro` VARCHAR(45) NOT NULL,
  `valore` FLOAT(8) NOT NULL,
  PRIMARY KEY (`esame`, `paziente`, `data`, `parametro`),
  INDEX `fk_Esame_effettivo_has_Parametro_Parametro1_idx` (`parametro` ASC) VISIBLE,
  INDEX `fk_Esame_effettivo_has_Parametro_Esame_effettivo1_idx` (`esame` ASC, `paziente` ASC, `data` ASC) VISIBLE,
  CONSTRAINT `fk_Esame_effettivo_has_Parametro_Esame_effettivo1`
    FOREIGN KEY (`esame` , `paziente` , `data`)
    REFERENCES `ASL_db`.`Esame_effettivo` (`examID` , `paziente` , `data`)
    ON DELETE CASCADE
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_Esame_effettivo_has_Parametro_Parametro1`
    FOREIGN KEY (`parametro`)
    REFERENCES `ASL_db`.`Parametro` (`nome`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;

USE `ASL_db` ;

-- -----------------------------------------------------
-- procedure inserisci_ospedale
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`inserisci_ospedale`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `inserisci_ospedale` (IN var_nome varchar(45), IN var_indirizzo varchar(100), IN var_primario VARCHAR(45), OUT var_hospID int)
BEGIN
	declare var_tipo enum('medico', 'primario', 'volontario');
    
	declare exit handler for sqlexception
		begin
			rollback;
			resignal;
		end;
        
	set transaction isolation level READ COMMITTED;
    start transaction;
		
        select P.tipo
        from Personale_anagrafica P
        where P.CF = var_primario
        into var_tipo;
        
        if var_tipo = 'volontario' then
			signal sqlstate '45006' set message_text = "Un volontario non può essere primario";
		elseif var_tipo = 'medico' then
			update Personale_anagrafica
            set tipo = 'primario'
            where CF = var_primario;
		end if;
        -- if var_tipo = 'primario' do nothing
        
		insert into Ospedale (nome, indirizzo, primario) 
		values (var_nome, var_indirizzo, var_primario);
    
		set var_hospID = last_insert_id();
	commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure inserisci_reparto
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`inserisci_reparto`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `inserisci_reparto` (IN var_hospID int, IN var_repID int, IN var_nome varchar(45), IN var_tel varchar(45))
BEGIN
	insert into Reparto (hospID, repID, nome, tel)
    values (var_hospID, var_repID, var_nome, var_tel);

END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure inserisci_laboratorio
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`inserisci_laboratorio`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `inserisci_laboratorio` (IN var_ospedale int, IN var_laboratorio int, IN var_nome varchar(45), IN var_piano int, IN var_stanza varchar(45),IN var_primario varchar(45))
BEGIN
	declare var_tipo enum('medico', 'primario', 'volontario');
    
    declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    set transaction isolation level read committed;
    start transaction;
		
        select tipo
        from Personale_anagrafica
        where CF = var_primario
        into var_tipo;
        
        if var_tipo = 'volontario' then
			signal sqlstate '45006' set message_text = "Un volontario non può essere un primario.";
		elseif var_tipo = 'medico' then
			update Personale_anagrafica
            set tipo = 'primario'
            where CF = var_primario;
		end if;
        
        -- if var_tipo = 'primario' do nothing;
    
		insert into Laboratorio(hospID, labID,nome, piano, stanza, primario) 
        values (var_ospedale, var_laboratorio, var_nome, var_piano, var_stanza, var_primario);

	commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure inserisci_paziente
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`inserisci_paziente`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `inserisci_paziente` (IN var_tessera_sanitaria varchar(45), IN var_nome varchar(45), IN var_cognome varchar(45), IN var_indirizzo varchar(100), IN var_dataN date, IN var_luogoN varchar(45))
BEGIN
	insert into Paziente (tessera_sanitaria, nome, cognome, indirizzo, dataN, luogoN)
    values (var_tessera_sanitaria, var_nome, var_cognome, var_indirizzo, var_dataN, var_luogoN);
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure crea_parametro
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`crea_parametro`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `crea_parametro` (IN var_nome varchar(45))
BEGIN
	insert into Parametro (nome) values (var_nome);
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure crea_tipologia_esame
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`crea_tipologia_esame`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `crea_tipologia_esame` (IN var_descrizione varchar(100), IN var_costo float, OUT var_examID int)
BEGIN
	insert into Esame(descrizione, costo) values (var_descrizione, var_costo);
    set var_examID = last_insert_id();
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure login
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`login`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `login` (IN var_username VARCHAR(45), IN var_password CHAR(32), OUT var_role INT)
BEGIN
	declare var_role_name ENUM('amministratore','personaleCUP');
    
    SELECT ruolo
    FROM Users 
    WHERE username = var_username AND password = md5(var_password)
    INTO var_role_name;
    
    -- protocol between DBMS and client
    IF var_role_name = 'amministratore' then
		set var_role = 1;
	ELSEIF var_role_name = 'personaleCUP' then
		set var_role = 2;
	ELSE
		set var_role = 3;
	END IF;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure registration
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`registration`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `registration` (IN var_username VARCHAR(45), IN var_password VARCHAR(32), IN var_ruolo ENUM('amministratore', 'personaleCUP'))
BEGIN
	INSERT INTO Users(username, password, ruolo) 
    VALUES (var_username, md5(var_password), var_ruolo);
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure aggiungi_email
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`aggiungi_email`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `aggiungi_email` (IN var_posta VARCHAR(45), IN var_paziente VARCHAR(45))
BEGIN
	INSERT INTO Email(posta, paziente)
    VALUES (var_posta, var_paziente);

END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure aggiungi_recapito_tel
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`aggiungi_recapito_tel`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `aggiungi_recapito_tel` (IN var_numero varchar(45), IN var_tipo ENUM('cellulare', 'fisso'), IN var_paziente VARCHAR(45))
BEGIN
	insert into Telefono (numero, tipo, paziente)
    values (var_numero, var_tipo, var_paziente);
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure add_personale_anagrafica
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`add_personale_anagrafica`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `add_personale_anagrafica` (IN var_CF VARCHAR(45), IN var_nome VARCHAR(45), IN var_cognome VARCHAR(45), IN var_indirizzo VARCHAR(100), IN var_tipo ENUM('medico','primario','volontario'), IN var_associazione VARCHAR(45))
BEGIN
	insert into Personale_anagrafica(CF, nome, cognome, indirizzo, tipo, associazione)
    values (var_CF, var_nome, var_cognome, var_indirizzo, var_tipo, var_associazione);
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure add_personale_lavorativo
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`add_personale_lavorativo`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `add_personale_lavorativo` (IN var_CF VARCHAR(45), IN var_ospedale INT, IN var_reparto INT)
BEGIN
	insert into Personale_dati_lavorativi (personale, ospedale, reparto)
    values (var_CF, var_ospedale, var_reparto);
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure aggiungi_tipo_specializzazione
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`aggiungi_tipo_specializzazione`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `aggiungi_tipo_specializzazione` (IN var_titolo VARCHAR(100))
BEGIN
	insert into Specializzazione (titolo)
    values (var_titolo);
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure assegna_specializzazione
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`assegna_specializzazione`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `assegna_specializzazione` (IN var_primario VARCHAR(45), IN var_specializzazione VARCHAR(100))
BEGIN
	insert into Specializzato (primario, specializzazione)
    values (var_primario, var_specializzazione);
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure add_parametro_ad_esame
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`add_parametro_ad_esame`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `add_parametro_ad_esame` (IN var_esame int, IN var_parametro varchar(45))
BEGIN
	insert into Composizione (examID, parametro)
    values (var_esame, var_parametro);
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure select_personale_by_hosp
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`select_personale_by_hosp`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `select_personale_by_hosp` (IN var_ospedale int)
BEGIN
	
    declare exit handler for sqlexception
		begin
			rollback;
			resignal;
		end;
        
	set transaction isolation level READ COMMITTED;
    start transaction;
    
		select P2.ospedale as ospedale, P2.reparto as reparto, CF, nome, cognome, indirizzo, tipo as ruolo, associazione
        from Personale_anagrafica as P1 join Personale_dati_lavorativi as P2 on P1.CF = P2.personale
		where P2.ospedale = var_ospedale;
        
    commit;
    
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure select_personale_by_rep
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`select_personale_by_rep`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `select_personale_by_rep` (IN var_ospedale int, IN var_reparto int)
BEGIN
	
    declare exit handler for sqlexception
		begin
			rollback;
			resignal;
		end;
        
	set transaction isolation level READ COMMITTED;
    start transaction;
    
		select P2.ospedale as ospedale, P2.reparto as reparto, CF, nome, cognome, indirizzo, tipo as ruolo, associazione
        from Personale_anagrafica as P1 join Personale_dati_lavorativi as P2 on P1.CF = P2.personale
		where P2.ospedale = var_ospedale and P2.reparto = var_reparto;
        
    commit;
    
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure prenota_esame
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`prenota_esame`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `prenota_esame` (IN var_esame int, IN var_paziente varchar(45), IN var_data date, IN var_ora time, IN var_urgenza enum('si', 'no'), IN var_diagnosi varchar(256), IN var_codiceP int, IN var_lab int, IN var_hosp int)
BEGIN

	insert into Esame_effettivo(examID, paziente, data, ora, urgenza, diagnosi, codiceP, laboratorio, ospedale)
    values (var_esame, var_paziente, var_data, var_ora, var_urgenza, var_diagnosi, var_codiceP, var_lab, var_hosp);
    
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure scrivi_diagnosi
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`scrivi_diagnosi`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `scrivi_diagnosi` (IN var_esame int, IN var_paziente varchar(45), IN var_data date, IN var_diagnosi varchar(256))
BEGIN

	update Esame_effettivo
    set diagnosi = var_diagnosi
    where examId = var_esame and paziente = var_paziente and data = var_data;
    
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure search_by_codiceP
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`search_by_codiceP`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `search_by_codiceP` (IN var_codiceP int)
BEGIN

	declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    set transaction isolation level read committed;
    start transaction;
		
        select paziente, descrizione as esame, data, ora, urgenza, diagnosi, ospedale, laboratorio
        from Esame_effettivo E1 join Esame E2 on E1.examID = E2.examID
        where E1.codiceP = var_codiceP;
        
    commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure inserisci_risultato_esame
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`inserisci_risultato_esame`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `inserisci_risultato_esame` (IN var_esame int, IN var_paziente varchar(45), IN var_data date, IN var_parametro varchar(45), IN var_valore float)
BEGIN
	insert into Risultato(esame, paziente, data, parametro, valore)
    values (var_esame, var_paziente, var_data, var_parametro, var_valore);
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure report_paziente
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`report_paziente`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `report_paziente` (IN var_paziente varchar(45))
BEGIN

	declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    set transaction isolation level READ COMMITTED;
    start transaction;
    
		select codiceP as 'P. code', descrizione as esame, data, urgenza, diagnosi, laboratorio, ospedale, costo
        from Esame_effettivo E join Esame E1 on E.examID = E1.examID
        where E.paziente = var_paziente and E.data <= current_date();
        
        select codiceP as 'P. code', descrizione as esame, data, urgenza, diagnosi, laboratorio, ospedale, costo
        from Esame_effettivo E join Esame E1 on E.examID = E1.examID
        where E.paziente = var_paziente and E.data > current_date();
        
    commit;

END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure assegna_personale_ad_esame
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`assegna_personale_ad_esame`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `assegna_personale_ad_esame` (IN var_esame int, IN var_paziente varchar(45), IN var_data date, IN var_personale varchar(45))
BEGIN

	insert into Svolgimento(esame, paziente, data, personale)
    values (var_esame, var_paziente, var_data, var_personale);
    
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure report_personale_mese
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`report_personale_mese`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `report_personale_mese` (IN var_personale varchar(45), OUT numero_esami int)
BEGIN

	declare var_qta_esami int;
    declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    -- consecutive reads and range lock needed
    set transaction isolation level SERIALIZABLE;
    start transaction;
		
         -- number of executed exams
        select count(*)
        from Svolgimento S
        where S.personale = var_personale and S.data <= current_date() and S.data >= DATE_SUB(current_date(), interval 1 MONTH)
        into numero_esami;
        
        -- info about executed exams
        select E1.descrizione as esame, E.paziente as paziente, E.data as data, E.urgenza as urgenza, E.ospedale as ospedale, E.laboratorio as laboratorio
        from Svolgimento S join Personale_anagrafica P on S.personale = P.CF
			join Esame_effettivo E on S.esame = E.examID and S.data = E.data and S.paziente = E.paziente
            join Esame E1 on E.examID = E1.examID
		where S.personale = var_personale and S.data <= current_date() and S.data >= DATE_SUB(current_date(), interval 1 MONTH);
        
       
        
    commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure report_personale_anno
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`report_personale_anno`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `report_personale_anno` (IN var_personale varchar(45), OUT numero_esami int)
BEGIN

	declare var_qta_esami int;
    declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    set transaction isolation level SERIALIZABLE;
    start transaction;
        
        -- number of executed exams
        select count(*)
        from Svolgimento S
        where S.personale = var_personale and S.data <= current_date() and S.data >= DATE_SUB(current_date(), interval 1 YEAR)
        into numero_esami;
        
        -- info about executed exams
        select E1.descrizione as esame, E.paziente as paziente, E.data as data, E.urgenza as urgenza, E.ospedale as ospedale, E.laboratorio as laboratorio
        from Svolgimento S join Personale_anagrafica P on S.personale = P.CF
			join Esame_effettivo E on S.esame = E.examID and S.data = E.data and S.paziente = E.paziente
            join Esame E1 on E.examID = E1.examID
		where S.personale = var_personale and S.data <= current_date() and S.data >= DATE_SUB(current_date(), interval 1 YEAR);
        
         
        
    commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure list_esami_disponibili
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`list_esami_disponibili`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `list_esami_disponibili` ()
BEGIN
	declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    set transaction isolation level READ COMMITTED;
    start transaction;
		select examID as ID, descrizione, costo
		from Esame;
	commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure list_ospedali
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`list_ospedali`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `list_ospedali` ()
BEGIN
	declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    set transaction isolation level READ COMMITTED;
    start transaction;
		select hospID as ID, O.nome as nome, O.indirizzo as indirizzo, P.nome as 'nome primario', P.cognome as 'cognome primario' 
		from Ospedale O join Personale_anagrafica P on O.primario = P.CF
        order by hospID asc;
	commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure list_laboratori
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`list_laboratori`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `list_laboratori` ()
BEGIN
	declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    set transaction isolation level READ COMMITTED;
    start transaction;
		select hospID as ospedale, labID as ID, Laboratorio.nome as nome, piano, stanza, P.nome as 'nome primario', P.cognome as 'cognome primario' 
		from Laboratorio join Personale_anagrafica P on primario = P.CF
        order by hospID asc;
	commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure list_reparti
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`list_reparti`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `list_reparti` (IN var_ospedale int)
BEGIN
	declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    set transaction isolation level READ COMMITTED;
    start transaction;
		select hospID as ospedale, repID as ID, nome, tel as telefono
		from Reparto
        where hospID = var_ospedale
        order by hospID asc;
	commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure list_patients
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`list_patients`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `list_patients` ()
BEGIN
	declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    set transaction isolation level READ COMMITTED;
		
        select tessera_sanitaria as 'tessera sanitaria', nome, cognome, indirizzo, dataN as 'D.O.B.', luogoN as 'luogo nascita'
        from Paziente
        order by tessera_sanitaria;
        
    commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure search_patient
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`search_patient`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `search_patient` (IN var_tessera_sanitaria varchar(45))
BEGIN

	declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    set transaction isolation level READ COMMITTED;
    start transaction;
    
		select tessera_sanitaria as 'tessera sanitaria', nome, cognome, indirizzo, dataN as 'D.O.B.', luogoN as 'luogo nascita'
        from Paziente
        where tessera_sanitaria = var_tessera_sanitaria;
        
    commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure search_recapiti
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`search_recapiti`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `search_recapiti` (IN var_paziente varchar(45))
BEGIN

	declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    set transaction isolation level READ COMMITTED;
    start transaction;
		
        select paziente, numero as recapito
        from Telefono
        where paziente = var_paziente
        union
        select paziente, posta as recapito
        from Email
        where paziente = var_paziente;
    commit;
        
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure search_strutture_primario
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`search_strutture_primario`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `search_strutture_primario` (IN var_primario varchar(45))
BEGIN

	declare var_ruolo enum('medico', 'primario', 'volontario');
	declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    set transaction isolation level READ COMMITTED;
    start transaction;
    
		select tipo
        from Personale_anagrafica
        where CF = var_primario
        into var_ruolo;
        
        if var_ruolo <> 'primario' then
			signal sqlstate '45009' set message_text = "Il membro del personale non è un primario";
		end if;
        
        
        select hospID as ospedale, nome, indirizzo
        from Ospedale
        where primario = var_primario;
        
        select hospID as ospedale, labID as laboratorio, nome, piano, stanza
        from Laboratorio
        where primario = var_primario;
        
    commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure report_risultati_prenotazione
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`report_risultati_prenotazione`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `report_risultati_prenotazione` (IN var_codiceP int)
BEGIN
    
	declare done int default false;
    declare var_examID int;
    declare var_paziente varchar(45);
    declare var_data date;
        
	declare cur cursor for
		select examID, paziente, data
        from Esame_effettivo
        where codiceP = var_codiceP;
	
	declare continue handler for not found
		set done = true;
    
    -- both handler must be declared after cursor
    declare exit handler for sqlexception
	begin
		rollback;
        resignal;
	end;
    
    set transaction isolation level REPEATABLE READ;
    start transaction;
		open cur;
		
		read_loop: loop
			fetch cur into var_examID, var_paziente, var_data;
			if done then
				leave read_loop;
			end if;
			
			select E.descrizione as esame, R.paziente as paziente, R.data as data, C.parametro as parametro, R.valore as valore
			from Esame E join Risultato R on E.examID = R.esame 
				right join Composizione C on R.esame = C.examID and R.parametro = C.parametro
			where R.esame = var_examID and R.paziente = var_paziente and R.data = var_data;
			
		end loop;   
		close cur;
        
    commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure cancella_prenotazione
-- -----------------------------------------------------

USE `ASL_db`;
DROP procedure IF EXISTS `ASL_db`.`cancella_prenotazione`;

DELIMITER $$
USE `ASL_db`$$
CREATE PROCEDURE `cancella_prenotazione` (IN var_esame int, IN var_paziente varchar(45), IN var_data date)
BEGIN

	delete from Esame_effettivo
    where examID = var_esame and paziente = var_paziente and data = var_data;
    
END$$

DELIMITER ;
USE `ASL_db`;

DELIMITER $$

USE `ASL_db`$$
DROP TRIGGER IF EXISTS `ASL_db`.`Esame_effettivo_BEFORE_INSERT` $$
USE `ASL_db`$$
CREATE DEFINER = CURRENT_USER TRIGGER `ASL_db`.`Esame_effettivo_BEFORE_INSERT` BEFORE INSERT ON `Esame_effettivo` FOR EACH ROW
BEGIN
	if NEW.data < CURDATE() then
		signal sqlstate '45007' set message_text = "A reservation in the past is not allowed!";
	end if;
END$$


USE `ASL_db`$$
DROP TRIGGER IF EXISTS `ASL_db`.`Esame_effettivo_BEFORE_INSERT_1` $$
USE `ASL_db`$$
CREATE DEFINER = CURRENT_USER TRIGGER `ASL_db`.`Esame_effettivo_BEFORE_INSERT_1` BEFORE INSERT ON `Esame_effettivo` FOR EACH ROW
BEGIN
	declare var_paziente varchar(45);
    
    select paziente 
    from Esame_effettivo
    where codiceP = NEW.codiceP
    into var_paziente;
    
    if var_paziente is not null and var_paziente <> NEW.paziente then	
		signal sqlstate '45012' set message_text="Il codice di prenotazione è già stato utilizzato per un altro paziente";
	end if;
END$$


USE `ASL_db`$$
DROP TRIGGER IF EXISTS `ASL_db`.`Esame_effettivo_BEFORE_DELETE` $$
USE `ASL_db`$$
CREATE DEFINER = CURRENT_USER TRIGGER `ASL_db`.`Esame_effettivo_BEFORE_DELETE` BEFORE DELETE ON `Esame_effettivo` FOR EACH ROW
BEGIN
	if OLD.data < CURDATE() then
		signal sqlstate '45011' set message_text = "L'esame è già stato eseguito e non può essere rimosso";
	end if;
END$$


USE `ASL_db`$$
DROP TRIGGER IF EXISTS `ASL_db`.`Svolgimento_BEFORE_INSERT` $$
USE `ASL_db`$$
CREATE DEFINER = CURRENT_USER TRIGGER `ASL_db`.`Svolgimento_BEFORE_INSERT` BEFORE INSERT ON `Svolgimento` FOR EACH ROW
BEGIN
	if NEW.data < current_date() then
		signal sqlstate '45008' set message_text = "Exam already executed";
	end if;
END$$


USE `ASL_db`$$
DROP TRIGGER IF EXISTS `ASL_db`.`Specializzato_BEFORE_INSERT` $$
USE `ASL_db`$$
CREATE DEFINER = CURRENT_USER TRIGGER `ASL_db`.`Specializzato_BEFORE_INSERT` BEFORE INSERT ON `Specializzato` FOR EACH ROW
BEGIN
	declare var_tipo enum('medico', 'primario', 'volontario');
    
    select tipo
    from Personale_anagrafica as P
    where P.CF = NEW.primario
    into var_tipo;
    
    if var_tipo <> 'primario' then	
		signal sqlstate '45005' set message_text = "Il membro del personale indicato non è un primario.";
	end if;
END$$


USE `ASL_db`$$
DROP TRIGGER IF EXISTS `ASL_db`.`Risultato_BEFORE_INSERT` $$
USE `ASL_db`$$
CREATE DEFINER = CURRENT_USER TRIGGER `ASL_db`.`Risultato_BEFORE_INSERT` BEFORE INSERT ON `Risultato` FOR EACH ROW
BEGIN

	declare counter int;
    
    select count(*)
    from Composizione C
    where NEW.esame = C.examID and NEW.parametro = C.parametro
    into counter;
    
    if counter <> 1 then
		signal sqlstate '45010' set message_text = "Parametro non previsto per tale esame.";
	end if;
    
END$$


DELIMITER ;
SET SQL_MODE = '';
DROP USER IF EXISTS login;
SET SQL_MODE='ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION';
CREATE USER 'login' IDENTIFIED BY 'loginUser0!';

GRANT EXECUTE ON procedure `ASL_db`.`login` TO 'login';
SET SQL_MODE = '';
DROP USER IF EXISTS personaleCUP;
SET SQL_MODE='ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION';
CREATE USER 'personaleCUP' IDENTIFIED BY 'personaleCUP0!';

GRANT EXECUTE ON procedure `ASL_db`.`inserisci_paziente` TO 'personaleCUP';
GRANT EXECUTE ON procedure `ASL_db`.`aggiungi_email` TO 'personaleCUP';
GRANT EXECUTE ON procedure `ASL_db`.`aggiungi_recapito_tel` TO 'personaleCUP';
GRANT EXECUTE ON procedure `ASL_db`.`prenota_esame` TO 'personaleCUP';
GRANT EXECUTE ON procedure `ASL_db`.`scrivi_diagnosi` TO 'personaleCUP';
GRANT EXECUTE ON procedure `ASL_db`.`search_by_codiceP` TO 'personaleCUP';
GRANT EXECUTE ON procedure `ASL_db`.`inserisci_risultato_esame` TO 'personaleCUP';
GRANT EXECUTE ON procedure `ASL_db`.`report_paziente` TO 'personaleCUP';
GRANT EXECUTE ON procedure `ASL_db`.`assegna_personale_ad_esame` TO 'personaleCUP';
GRANT EXECUTE ON procedure `ASL_db`.`list_esami_disponibili` TO 'personaleCUP';
GRANT EXECUTE ON procedure `ASL_db`.`list_ospedali` TO 'personaleCUP';
GRANT EXECUTE ON procedure `ASL_db`.`list_laboratori` TO 'personaleCUP';
GRANT EXECUTE ON procedure `ASL_db`.`list_reparti` TO 'personaleCUP';
GRANT EXECUTE ON procedure `ASL_db`.`select_personale_by_hosp` TO 'personaleCUP';
GRANT EXECUTE ON procedure `ASL_db`.`select_personale_by_rep` TO 'personaleCUP';
GRANT EXECUTE ON procedure `ASL_db`.`list_patients` TO 'personaleCUP';
GRANT EXECUTE ON procedure `ASL_db`.`search_patient` TO 'personaleCUP';
GRANT EXECUTE ON procedure `ASL_db`.`search_recapiti` TO 'personaleCUP';
GRANT EXECUTE ON procedure `ASL_db`.`report_risultati_prenotazione` TO 'personaleCUP';
GRANT EXECUTE ON procedure `ASL_db`.`cancella_prenotazione` TO 'personaleCUP';
SET SQL_MODE = '';
DROP USER IF EXISTS amministratore;
SET SQL_MODE='ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION';
CREATE USER 'amministratore' IDENTIFIED BY 'Amministratore0!';

GRANT EXECUTE ON procedure `ASL_db`.`registration` TO 'amministratore';
GRANT EXECUTE ON procedure `ASL_db`.`crea_parametro` TO 'amministratore';
GRANT EXECUTE ON procedure `ASL_db`.`crea_tipologia_esame` TO 'amministratore';
GRANT EXECUTE ON procedure `ASL_db`.`add_personale_anagrafica` TO 'amministratore';
GRANT EXECUTE ON procedure `ASL_db`.`add_personale_lavorativo` TO 'amministratore';
GRANT EXECUTE ON procedure `ASL_db`.`aggiungi_tipo_specializzazione` TO 'amministratore';
GRANT EXECUTE ON procedure `ASL_db`.`assegna_specializzazione` TO 'amministratore';
GRANT EXECUTE ON procedure `ASL_db`.`inserisci_ospedale` TO 'amministratore';
GRANT EXECUTE ON procedure `ASL_db`.`inserisci_laboratorio` TO 'amministratore';
GRANT EXECUTE ON procedure `ASL_db`.`inserisci_reparto` TO 'amministratore';
GRANT EXECUTE ON procedure `ASL_db`.`add_parametro_ad_esame` TO 'amministratore';
GRANT EXECUTE ON procedure `ASL_db`.`select_personale_by_hosp` TO 'amministratore';
GRANT EXECUTE ON procedure `ASL_db`.`select_personale_by_rep` TO 'amministratore';
GRANT EXECUTE ON procedure `ASL_db`.`report_personale_mese` TO 'amministratore';
GRANT EXECUTE ON procedure `ASL_db`.`report_personale_anno` TO 'amministratore';
GRANT EXECUTE ON procedure `ASL_db`.`list_ospedali` TO 'amministratore';
GRANT EXECUTE ON procedure `ASL_db`.`list_laboratori` TO 'amministratore';
GRANT EXECUTE ON procedure `ASL_db`.`list_reparti` TO 'amministratore';
GRANT EXECUTE ON procedure `ASL_db`.`list_esami_disponibili` TO 'amministratore';
GRANT EXECUTE ON procedure `ASL_db`.`search_strutture_primario` TO 'amministratore';

SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;

-- -----------------------------------------------------
-- Data for table `ASL_db`.`Paziente`
-- -----------------------------------------------------
START TRANSACTION;
USE `ASL_db`;
INSERT INTO `ASL_db`.`Paziente` (`tessera_sanitaria`, `nome`, `cognome`, `indirizzo`, `dataN`, `luogoN`) VALUES ('A', 'Amerigo', 'Vespucci', 'Madrid, Via delle Indie, 107', '1964-08-23', 'Barcellona');
INSERT INTO `ASL_db`.`Paziente` (`tessera_sanitaria`, `nome`, `cognome`, `indirizzo`, `dataN`, `luogoN`) VALUES ('B', 'Boris', 'Diaw', 'Tolosa, Rue Eiffel, 12', '1986-10-04', 'Parigi');

COMMIT;


-- -----------------------------------------------------
-- Data for table `ASL_db`.`Esame`
-- -----------------------------------------------------
START TRANSACTION;
USE `ASL_db`;
INSERT INTO `ASL_db`.`Esame` (`examID`, `descrizione`, `costo`) VALUES (3, 'Emocromo', 35.8);
INSERT INTO `ASL_db`.`Esame` (`examID`, `descrizione`, `costo`) VALUES (4, 'RX', 10.5);
INSERT INTO `ASL_db`.`Esame` (`examID`, `descrizione`, `costo`) VALUES (1, 'Analisi del sangue', 5.5);
INSERT INTO `ASL_db`.`Esame` (`examID`, `descrizione`, `costo`) VALUES (2, 'Analisi delle urine', 12.35);

COMMIT;


-- -----------------------------------------------------
-- Data for table `ASL_db`.`Parametro`
-- -----------------------------------------------------
START TRANSACTION;
USE `ASL_db`;
INSERT INTO `ASL_db`.`Parametro` (`nome`) VALUES ('Transaminasi');
INSERT INTO `ASL_db`.`Parametro` (`nome`) VALUES ('Colesterolo');
INSERT INTO `ASL_db`.`Parametro` (`nome`) VALUES ('MCV');
INSERT INTO `ASL_db`.`Parametro` (`nome`) VALUES ('Emoglobina');
INSERT INTO `ASL_db`.`Parametro` (`nome`) VALUES ('Piastrine');
INSERT INTO `ASL_db`.`Parametro` (`nome`) VALUES ('Linfociti');
INSERT INTO `ASL_db`.`Parametro` (`nome`) VALUES ('Neutrofili');
INSERT INTO `ASL_db`.`Parametro` (`nome`) VALUES ('GammaVC');

COMMIT;


-- -----------------------------------------------------
-- Data for table `ASL_db`.`Personale_anagrafica`
-- -----------------------------------------------------
START TRANSACTION;
USE `ASL_db`;
INSERT INTO `ASL_db`.`Personale_anagrafica` (`CF`, `nome`, `cognome`, `indirizzo`, `tipo`, `associazione`) VALUES ('A', 'Andrea', 'Pepe', 'Roma, Via V. Spurinna, 105', 'primario', NULL);
INSERT INTO `ASL_db`.`Personale_anagrafica` (`CF`, `nome`, `cognome`, `indirizzo`, `tipo`, `associazione`) VALUES ('B', 'Bernardo', 'Silva', 'Torino, Via Giovanni XXIII, 24', 'volontario', 'UNICEF');
INSERT INTO `ASL_db`.`Personale_anagrafica` (`CF`, `nome`, `cognome`, `indirizzo`, `tipo`, `associazione`) VALUES ('C', 'Carmelo', 'Bene', 'Napoli, Via Caracciolo, 256', 'primario', NULL);

COMMIT;


-- -----------------------------------------------------
-- Data for table `ASL_db`.`Ospedale`
-- -----------------------------------------------------
START TRANSACTION;
USE `ASL_db`;
INSERT INTO `ASL_db`.`Ospedale` (`hospID`, `nome`, `indirizzo`, `primario`) VALUES (1, 'Gemelli', 'Roma, Via America, 23', 'A');
INSERT INTO `ASL_db`.`Ospedale` (`hospID`, `nome`, `indirizzo`, `primario`) VALUES (2, 'Spallanzani', 'Roma, Via Caligola, 258', 'C');

COMMIT;


-- -----------------------------------------------------
-- Data for table `ASL_db`.`Laboratorio`
-- -----------------------------------------------------
START TRANSACTION;
USE `ASL_db`;
INSERT INTO `ASL_db`.`Laboratorio` (`hospID`, `labID`, `nome`, `piano`, `stanza`, `primario`) VALUES (1, 1, 'Analisi', 3, 'D5', 'C');
INSERT INTO `ASL_db`.`Laboratorio` (`hospID`, `labID`, `nome`, `piano`, `stanza`, `primario`) VALUES (1, 2, 'Esami oncologici', 4, 'E3', 'C');
INSERT INTO `ASL_db`.`Laboratorio` (`hospID`, `labID`, `nome`, `piano`, `stanza`, `primario`) VALUES (2, 1, 'Analisi', 2, 'S3', 'A');

COMMIT;


-- -----------------------------------------------------
-- Data for table `ASL_db`.`Esame_effettivo`
-- -----------------------------------------------------
START TRANSACTION;
USE `ASL_db`;
INSERT INTO `ASL_db`.`Esame_effettivo` (`examID`, `paziente`, `data`, `ora`, `urgenza`, `diagnosi`, `codiceP`, `laboratorio`, `ospedale`) VALUES (1, 'A', '2021-04-12', '12:30:00', 'si', NULL, 1, 1, 1);
INSERT INTO `ASL_db`.`Esame_effettivo` (`examID`, `paziente`, `data`, `ora`, `urgenza`, `diagnosi`, `codiceP`, `laboratorio`, `ospedale`) VALUES (2, 'A', '2021-05-15', '13:22:00', 'no', NULL, 1, 1, 2);

COMMIT;


-- -----------------------------------------------------
-- Data for table `ASL_db`.`Composizione`
-- -----------------------------------------------------
START TRANSACTION;
USE `ASL_db`;
INSERT INTO `ASL_db`.`Composizione` (`examID`, `parametro`) VALUES (1, 'Colesterolo');
INSERT INTO `ASL_db`.`Composizione` (`examID`, `parametro`) VALUES (2, 'GammaVC');
INSERT INTO `ASL_db`.`Composizione` (`examID`, `parametro`) VALUES (1, 'Linfociti');
INSERT INTO `ASL_db`.`Composizione` (`examID`, `parametro`) VALUES (2, 'Linfociti');
INSERT INTO `ASL_db`.`Composizione` (`examID`, `parametro`) VALUES (1, 'Piastrine');
INSERT INTO `ASL_db`.`Composizione` (`examID`, `parametro`) VALUES (1, 'Transaminasi');

COMMIT;


-- -----------------------------------------------------
-- Data for table `ASL_db`.`Reparto`
-- -----------------------------------------------------
START TRANSACTION;
USE `ASL_db`;
INSERT INTO `ASL_db`.`Reparto` (`hospID`, `repID`, `nome`, `tel`) VALUES (1, 1, 'Pediatria', '1234567890');
INSERT INTO `ASL_db`.`Reparto` (`hospID`, `repID`, `nome`, `tel`) VALUES (1, 2, 'Cardiologia', '2345126897');
INSERT INTO `ASL_db`.`Reparto` (`hospID`, `repID`, `nome`, `tel`) VALUES (1, 3, 'Radiologia', '1234589670');
INSERT INTO `ASL_db`.`Reparto` (`hospID`, `repID`, `nome`, `tel`) VALUES (2, 1, 'Radiologia', '6734598722');
INSERT INTO `ASL_db`.`Reparto` (`hospID`, `repID`, `nome`, `tel`) VALUES (2, 2, 'Oncologia', '4556677890');

COMMIT;


-- -----------------------------------------------------
-- Data for table `ASL_db`.`Svolgimento`
-- -----------------------------------------------------
START TRANSACTION;
USE `ASL_db`;
INSERT INTO `ASL_db`.`Svolgimento` (`esame`, `paziente`, `data`, `personale`) VALUES (1, 'A', '2021-04-12', 'A');

COMMIT;


-- -----------------------------------------------------
-- Data for table `ASL_db`.`Specializzazione`
-- -----------------------------------------------------
START TRANSACTION;
USE `ASL_db`;
INSERT INTO `ASL_db`.`Specializzazione` (`titolo`) VALUES ('Cardiologia');
INSERT INTO `ASL_db`.`Specializzazione` (`titolo`) VALUES ('Pediatria');
INSERT INTO `ASL_db`.`Specializzazione` (`titolo`) VALUES ('Medicina Sportiva');

COMMIT;


-- -----------------------------------------------------
-- Data for table `ASL_db`.`Users`
-- -----------------------------------------------------
START TRANSACTION;
USE `ASL_db`;
INSERT INTO `ASL_db`.`Users` (`username`, `password`, `ruolo`) VALUES ('admin', 'c6009f08fc5fc6385f1ea1f5840e179f', 'amministratore');
INSERT INTO `ASL_db`.`Users` (`username`, `password`, `ruolo`) VALUES ('prova', 'c6009f08fc5fc6385f1ea1f5840e179f', 'personaleCUP');

COMMIT;


-- -----------------------------------------------------
-- Data for table `ASL_db`.`Personale_dati_lavorativi`
-- -----------------------------------------------------
START TRANSACTION;
USE `ASL_db`;
INSERT INTO `ASL_db`.`Personale_dati_lavorativi` (`personale`, `ospedale`, `reparto`) VALUES ('C', 1, 1);
INSERT INTO `ASL_db`.`Personale_dati_lavorativi` (`personale`, `ospedale`, `reparto`) VALUES ('B', 1, 3);
INSERT INTO `ASL_db`.`Personale_dati_lavorativi` (`personale`, `ospedale`, `reparto`) VALUES ('A', 2, 2);

COMMIT;


-- -----------------------------------------------------
-- Data for table `ASL_db`.`Risultato`
-- -----------------------------------------------------
START TRANSACTION;
USE `ASL_db`;
INSERT INTO `ASL_db`.`Risultato` (`esame`, `paziente`, `data`, `parametro`, `valore`) VALUES (1, 'A', '2021-04-12', 'Colesterolo', 156.23);
INSERT INTO `ASL_db`.`Risultato` (`esame`, `paziente`, `data`, `parametro`, `valore`) VALUES (1, 'A', '2021-04-12', 'Piastrine', 45.78);
INSERT INTO `ASL_db`.`Risultato` (`esame`, `paziente`, `data`, `parametro`, `valore`) VALUES (2, 'A', '2021-05-15', 'GammaVC', 206);

COMMIT;


