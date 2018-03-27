startup<-read.table("/home/wyatt/Documents/Arboreal/Source/DataDone/rename_tag_time.txt", header = TRUE)


plot(time~files, data = startup)
plot(time~tags, data = startup)
plot(time~associatedFiles, data = startup)


fit<-lm(time~associatedFiles + I(associatedFiles^3), data = startup)
summary(fit)
plot(fit)


plot(fit$residuals~startup$associatedFiles,ylab="Residuals", xlab="Files", main="Files Residual Plot")
