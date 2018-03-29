startup<-read.table("/home/wyatt/Documents/Arboreal/Source/DataNormal/rename_tag_time.txt", header = TRUE)


startup<-startup[startup$files > 0,]
startup<-startup[startup$associatedfiles > 0,]

plot(log(time)~files, data = startup)
plot(log(time)~tags, data = startup)
plot(log(time)~associatedfiles, data = startup)


fit<-lm(log(time)~associatedfiles:files + files + I(associatedfiles^2) + log(associatedfiles) 
        + log(files) + log(tags), data = startup)
summary(fit)
plot(fit)


plot(fit$residuals~startup$associatedfiles,ylab="Residuals", xlab="Files", main="Files Residual Plot")
plot(fit$residuals~startup$tags,ylab="Residuals", xlab="Files", main="Files Residual Plot")
plot(fit$residuals~startup$files,ylab="Residuals", xlab="Files", main="Files Residual Plot")
